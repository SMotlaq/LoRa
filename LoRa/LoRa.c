/**
 * @file LoRa.c
 * @brief Implementation of Ra-02 / SX1278 LoRa library for STM32 HAL
 *
 * @see LoRa.h for public API documentation
 */

#include "LoRa.h"

/* -------------------------------------------------------------------------- */
/*                         Constructor / Defaults                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Create a new LoRa instance with safe default values.
 */
LoRa newLoRa(void)
{
    LoRa lora;

    lora.frequency              = 433;
    lora.spreadingFactor        = LORA_SF_7;
    lora.bandwidth              = LORA_BW_125_KHZ;
    lora.codingRate             = LORA_CR_4_5;
    lora.power                  = LORA_POWER_20DBM;
    lora.overCurrentProtection  = 100;
    lora.preamble               = 8;
    lora.current_mode           = LORA_MODE_STANDBY;

    /* Hardware pointers must be set by the user before LoRa_init() */
    lora.CS_port    = NULL;
    lora.CS_pin     = 0;
    lora.reset_port = NULL;
    lora.reset_pin  = 0;
    lora.DIO0_port  = NULL;
    lora.DIO0_pin   = 0;
    lora.hSPIx      = NULL;

    return lora;
}

/* -------------------------------------------------------------------------- */
/*                              Hardware Reset                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Perform a hardware reset via the RESET pin.
 */
void LoRa_reset(LoRa* _LoRa)
{
    if (_LoRa == NULL || _LoRa->reset_port == NULL) {
        return;
    }

    HAL_GPIO_WritePin(_LoRa->reset_port, _LoRa->reset_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(_LoRa->reset_port, _LoRa->reset_pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

/* -------------------------------------------------------------------------- */
/*                           Mode Management                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Change the operating mode of the radio.
 */
void LoRa_gotoMode(LoRa* _LoRa, int mode)
{
    uint8_t read, data;

    if (_LoRa == NULL) {
        return;
    }

    read = LoRa_read(_LoRa, REG_OP_MODE);
    data = (read & 0xF8U);          /* clear Mode bits */

    switch (mode) {
        case LORA_MODE_SLEEP:
            data |= 0x00;
            break;
        case LORA_MODE_STANDBY:
            data |= 0x01;
            break;
        case LORA_MODE_TRANSMIT:
            data |= 0x03;
            break;
        case LORA_MODE_RX_CONTINUOUS:
            data |= 0x05;
            break;
        case LORA_MODE_RX_SINGLE:
            data |= 0x06;
            break;
        default:
            return;                 /* invalid mode */
    }

    LoRa_write(_LoRa, REG_OP_MODE, data);
    _LoRa->current_mode = mode;
}

/* -------------------------------------------------------------------------- */
/*                        Low-level SPI Access                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Read one or more registers.
 */
void LoRa_readReg(LoRa* _LoRa, uint8_t* address, uint16_t addr_len,
                  uint8_t* output, uint16_t read_len)
{
    if (_LoRa == NULL || _LoRa->hSPIx == NULL || address == NULL || output == NULL) {
        return;
    }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(_LoRa->hSPIx, address, addr_len, LORA_TRANSMIT_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_SPI_Receive(_LoRa->hSPIx, output, read_len, LORA_RECEIVE_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
}

/**
 * @brief Write one or more registers.
 */
void LoRa_writeReg(LoRa* _LoRa, uint8_t* address, uint16_t addr_len,
                   uint8_t* values, uint16_t write_len)
{
    if (_LoRa == NULL || _LoRa->hSPIx == NULL || address == NULL || values == NULL) {
        return;
    }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(_LoRa->hSPIx, address, addr_len, LORA_TRANSMIT_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_SPI_Transmit(_LoRa->hSPIx, values, write_len, LORA_TRANSMIT_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
}

/**
 * @brief Read a single register.
 */
uint8_t LoRa_read(LoRa* _LoRa, uint8_t address)
{
    uint8_t data_addr = address & 0x7FU;
    uint8_t read_data = 0;

    LoRa_readReg(_LoRa, &data_addr, 1, &read_data, 1);
    return read_data;
}

/**
 * @brief Write a single register.
 */
void LoRa_write(LoRa* _LoRa, uint8_t address, uint8_t value)
{
    uint8_t addr = address | 0x80U;
    uint8_t data = value;

    LoRa_writeReg(_LoRa, &addr, 1, &data, 1);
}

/**
 * @brief Burst write (mainly used for FIFO payload).
 */
void LoRa_BurstWrite(LoRa* _LoRa, uint8_t address, uint8_t* value, uint8_t length)
{
    uint8_t addr = address | 0x80U;

    if (_LoRa == NULL || value == NULL || length == 0) {
        return;
    }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(_LoRa->hSPIx, &addr, 1, LORA_TRANSMIT_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_SPI_Transmit(_LoRa->hSPIx, value, length, LORA_TRANSMIT_TIMEOUT_MS);
    while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY) { /* wait */ }

    HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
}

/* -------------------------------------------------------------------------- */
/*                          Configuration Helpers                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Check that mandatory hardware pointers are set.
 */
uint8_t LoRa_isvalid(LoRa* _LoRa)
{
    if (_LoRa == NULL) {
        return 0;
    }
    if (_LoRa->CS_port == NULL || _LoRa->reset_port == NULL ||
        _LoRa->hSPIx == NULL) {
        return 0;
    }
    return 1;
}

/**
 * @brief Manually control Low Data Rate Optimization bit.
 */
void LoRa_setLowDataRateOptimization(LoRa* _LoRa, uint8_t value)
{
    uint8_t read, data;

    read = LoRa_read(_LoRa, REG_MODEM_CONFIG_3);

    if (value) {
        data = read | 0x08U;        /* set LDO bit */
    } else {
        data = read & 0xF7U;        /* clear LDO bit */
    }

    LoRa_write(_LoRa, REG_MODEM_CONFIG_3, data);
    HAL_Delay(10);
}

/**
 * @brief Automatically enable LDO when symbol time > 16 ms.
 */
void LoRa_setAutoLDO(LoRa* _LoRa)
{
    /* Bandwidth table corresponding to LORA_BW_xxx defines */
    static const double bw_khz[] = {
        7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0, 500.0
    };

    if (_LoRa->bandwidth > LORA_BW_500_KHZ) {
        return;
    }

    double symbol_time_ms = (double)(1UL << _LoRa->spreadingFactor) /
                            bw_khz[_LoRa->bandwidth];

    LoRa_setLowDataRateOptimization(_LoRa, (symbol_time_ms > 16.0) ? 1 : 0);
}

/**
 * @brief Set carrier frequency (MHz, integer).
 *
 * Formula: Frf = (freq_MHz * 2^19) / 32
 */
void LoRa_setFrequency(LoRa* _LoRa, int freq)
{
    uint32_t frf = ((uint32_t)freq * 524288UL) >> 5;   /* = freq * 16384 */

    LoRa_write(_LoRa, REG_FRF_MSB, (uint8_t)(frf >> 16));
    HAL_Delay(5);
    LoRa_write(_LoRa, REG_FRF_MID, (uint8_t)(frf >> 8));
    HAL_Delay(5);
    LoRa_write(_LoRa, REG_FRF_LSB, (uint8_t)(frf >> 0));
    HAL_Delay(5);
}

/**
 * @brief Set Spreading Factor (SF6 … SF12) and update LDO.
 */
void LoRa_setSpreadingFactor(LoRa* _LoRa, int sf)
{
    uint8_t read, data;

    if (sf < LORA_SF_6)  sf = LORA_SF_6;
    if (sf > LORA_SF_12) sf = LORA_SF_12;

    read = LoRa_read(_LoRa, REG_MODEM_CONFIG_2);
    HAL_Delay(10);

    /* SF in bits [7:4], keep lower nibble (CRC, TxCont, SymbTimeout MSB) */
    data = (uint8_t)((sf << 4) | (read & 0x0FU));
    LoRa_write(_LoRa, REG_MODEM_CONFIG_2, data);
    HAL_Delay(10);

    /* SF6 requires special Detection Optimize & Threshold registers
     * (not implemented here for simplicity – works for SF7-12 out of the box) */

    LoRa_setAutoLDO(_LoRa);
}

/**
 * @brief Set output power and configure PA_DAC for +20 dBm if needed.
 */
void LoRa_setPower(LoRa* _LoRa, uint8_t power)
{
    LoRa_write(_LoRa, REG_PA_CONFIG, power);

    if (power == LORA_POWER_20DBM) {
        LoRa_write(_LoRa, REG_PA_DAC, 0x87);   /* High-power mode */
    } else {
        LoRa_write(_LoRa, REG_PA_DAC, 0x84);   /* Normal mode (max +17 dBm) */
    }

    HAL_Delay(10);
}

/**
 * @brief Configure Over Current Protection.
 *
 * OcpTrim calculation follows Semtech datasheet.
 */
void LoRa_setOCP(LoRa* _LoRa, uint8_t current)
{
    uint8_t ocpTrim;

    if (current < 45)  current = 45;
    if (current > 240) current = 240;

    if (current <= 120) {
        ocpTrim = (current - 45) / 5;
    } else {
        ocpTrim = (current + 30) / 10;
    }

    ocpTrim |= (1U << 5);           /* OcpOn = 1 */
    LoRa_write(_LoRa, REG_OCP, ocpTrim);
    HAL_Delay(10);
}

/**
 * @brief Set symbol timeout MSB bits and enable payload CRC.
 */
void LoRa_setTOMsb_setCRCon(LoRa* _LoRa)
{
    uint8_t read = LoRa_read(_LoRa, REG_MODEM_CONFIG_2);
    /* bits [2:0] = SymbTimeout(9:8) + RxPayloadCrcOn */
    uint8_t data = read | 0x07U;
    LoRa_write(_LoRa, REG_MODEM_CONFIG_2, data);
    HAL_Delay(10);
}

/**
 * @brief Set the Sync Word (0x12 = private, 0x34 = public/LoRaWAN).
 */
void LoRa_setSyncWord(LoRa* _LoRa, uint8_t syncword)
{
    LoRa_write(_LoRa, REG_SYNC_WORD, syncword);
    HAL_Delay(10);
}

/* -------------------------------------------------------------------------- */
/*                         Transmit / Receive                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Blocking transmit with timeout.
 */
uint8_t LoRa_transmit(LoRa* _LoRa, uint8_t* data, uint8_t length, uint16_t timeout)
{
    uint8_t irqFlags;
    int previous_mode;

    if (_LoRa == NULL || data == NULL || length == 0 || length > 255) {
        return 0;
    }

    previous_mode = _LoRa->current_mode;

    LoRa_gotoMode(_LoRa, LORA_MODE_STANDBY);

    /* Set FIFO pointer to Tx base address */
    uint8_t txBase = LoRa_read(_LoRa, REG_FIFO_TX_BASE_ADDR);
    LoRa_write(_LoRa, REG_FIFO_ADDR_PTR, txBase);
    LoRa_write(_LoRa, REG_PAYLOAD_LENGTH, length);

    /* Write payload */
    LoRa_BurstWrite(_LoRa, REG_FIFO, data, length);

    /* Start transmission */
    LoRa_gotoMode(_LoRa, LORA_MODE_TRANSMIT);

    /* Wait for TxDone (bit 3 of IRQ flags) or timeout */
    while (timeout--) {
        irqFlags = LoRa_read(_LoRa, REG_IRQ_FLAGS);
        if (irqFlags & 0x08U) {                 /* TxDone */
            LoRa_write(_LoRa, REG_IRQ_FLAGS, 0xFF);  /* clear all flags */
            LoRa_gotoMode(_LoRa, previous_mode);
            return 1;
        }
        HAL_Delay(1);
    }

    /* Timeout */
    LoRa_gotoMode(_LoRa, previous_mode);
    return 0;
}

/**
 * @brief Enter continuous receive mode.
 */
void LoRa_startReceiving(LoRa* _LoRa)
{
    LoRa_gotoMode(_LoRa, LORA_MODE_RX_CONTINUOUS);
}

/**
 * @brief Polling receive – returns number of bytes received.
 */
uint8_t LoRa_receive(LoRa* _LoRa, uint8_t* data, uint8_t length)
{
    uint8_t irqFlags;
    uint8_t rxBytes;
    uint8_t fifoAddr;
    uint8_t bytesToRead;
    uint8_t i;

    if (_LoRa == NULL || data == NULL || length == 0) {
        return 0;
    }

    /* Clear output buffer */
    for (i = 0; i < length; i++) {
        data[i] = 0;
    }

    LoRa_gotoMode(_LoRa, LORA_MODE_STANDBY);

    irqFlags = LoRa_read(_LoRa, REG_IRQ_FLAGS);

    if (irqFlags & 0x40U) {                     /* RxDone */
        LoRa_write(_LoRa, REG_IRQ_FLAGS, 0xFF); /* clear flags */

        rxBytes  = LoRa_read(_LoRa, REG_RX_NB_BYTES);
        fifoAddr = LoRa_read(_LoRa, REG_FIFO_RX_CURRENT_ADDR);
        LoRa_write(_LoRa, REG_FIFO_ADDR_PTR, fifoAddr);

        bytesToRead = (rxBytes < length) ? rxBytes : length;

        for (i = 0; i < bytesToRead; i++) {
            data[i] = LoRa_read(_LoRa, REG_FIFO);
        }

        LoRa_gotoMode(_LoRa, LORA_MODE_RX_CONTINUOUS);
        return bytesToRead;
    }

    /* No packet */
    LoRa_gotoMode(_LoRa, LORA_MODE_RX_CONTINUOUS);
    return 0;
}

/**
 * @brief Interrupt-context receive (call from DIO0 rising-edge EXTI).
 *
 * This version assumes the module is already in RX Continuous mode
 * and DIO0 is mapped to RxDone.
 */
uint8_t LoRa_receive_IT(LoRa* _LoRa, uint8_t* data, uint8_t length)
{
    uint8_t irqFlags;
    uint8_t rxBytes;
    uint8_t fifoAddr;
    uint8_t bytesToRead;
    uint8_t i;

    if (_LoRa == NULL || data == NULL || length == 0) {
        return 0;
    }

    irqFlags = LoRa_read(_LoRa, REG_IRQ_FLAGS);

    if (!(irqFlags & 0x40U)) {                  /* not RxDone */
        return 0;
    }

    LoRa_write(_LoRa, REG_IRQ_FLAGS, 0xFF);     /* clear all IRQ flags */

    rxBytes  = LoRa_read(_LoRa, REG_RX_NB_BYTES);
    fifoAddr = LoRa_read(_LoRa, REG_FIFO_RX_CURRENT_ADDR);
    LoRa_write(_LoRa, REG_FIFO_ADDR_PTR, fifoAddr);

    bytesToRead = (rxBytes < length) ? rxBytes : length;

    for (i = 0; i < bytesToRead; i++) {
        data[i] = LoRa_read(_LoRa, REG_FIFO);
    }

    /* Stay in RX Continuous – do not change mode here */
    return bytesToRead;
}

/* -------------------------------------------------------------------------- */
/*                          Signal Quality                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief Return Packet RSSI in dBm (LF port offset = -164).
 *
 * For better accuracy when SNR < 0 the SNR contribution is added.
 */
int LoRa_getRSSI(LoRa* _LoRa)
{
    uint8_t rssi_raw = LoRa_read(_LoRa, REG_PKT_RSSI_VALUE);
    int8_t  snr_raw  = (int8_t)LoRa_read(_LoRa, REG_PKT_SNR_VALUE);
    float   snr_dB   = snr_raw * 0.25f;

    /* SX1278 is Low-Frequency port → offset -164 */
    int rssi = -164 + (int)rssi_raw;

    if (snr_dB < 0.0f) {
        rssi += (int)snr_dB;        /* correction when below noise floor */
    }

    return rssi;
}

/**
 * @brief Return Packet SNR in dB (resolution 0.25 dB).
 */
float LoRa_getSNR(LoRa* _LoRa)
{
    int8_t snr_raw = (int8_t)LoRa_read(_LoRa, REG_PKT_SNR_VALUE);
    return snr_raw * 0.25f;
}

/* -------------------------------------------------------------------------- */
/*                               Initialization                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Full initialization of the module according to the LoRa structure.
 */
uint16_t LoRa_init(LoRa* _LoRa)
{
    uint8_t read, data;

    if (!LoRa_isvalid(_LoRa)) {
        return LORA_UNAVAILABLE;
    }

    /* 1. Hardware reset */
    LoRa_reset(_LoRa);

    /* 2. Go to Sleep so we can set LongRangeMode bit */
    LoRa_gotoMode(_LoRa, LORA_MODE_SLEEP);
    HAL_Delay(10);

    /* 3. Enable LoRa mode (LongRangeMode = 1) */
    read = LoRa_read(_LoRa, REG_OP_MODE);
    data = read | 0x80U;
    LoRa_write(_LoRa, REG_OP_MODE, data);
    HAL_Delay(100);

    /* 4. Frequency */
    LoRa_setFrequency(_LoRa, _LoRa->frequency);

    /* 5. Output power */
    LoRa_setPower(_LoRa, _LoRa->power);

    /* 6. Over-current protection */
    LoRa_setOCP(_LoRa, _LoRa->overCurrentProtection);

    /* 7. LNA gain (max gain + boost) */
    LoRa_write(_LoRa, REG_LNA, 0x23);

    /* 8. CRC on + symbol timeout MSB */
    LoRa_setTOMsb_setCRCon(_LoRa);

    /* 9. Spreading Factor (+ auto LDO) */
    LoRa_setSpreadingFactor(_LoRa, _LoRa->spreadingFactor);

    /* 10. Symbol timeout LSB */
    LoRa_write(_LoRa, REG_SYMB_TIMEOUT_LSB, 0xFF);

    /* 11. Bandwidth + Coding Rate + Explicit Header */
    data = (uint8_t)((_LoRa->bandwidth << 4) | (_LoRa->codingRate << 1));
    /* bit 0 = 0 → Explicit Header Mode */
    LoRa_write(_LoRa, REG_MODEM_CONFIG_1, data);
    LoRa_setAutoLDO(_LoRa);

    /* 12. Preamble length */
    LoRa_write(_LoRa, REG_PREAMBLE_MSB, (uint8_t)(_LoRa->preamble >> 8));
    LoRa_write(_LoRa, REG_PREAMBLE_LSB, (uint8_t)(_LoRa->preamble & 0xFF));

    /* 13. DIO0 → RxDone (for interrupt-driven reception) */
    read = LoRa_read(_LoRa, REG_DIO_MAPPING_1);
    data = (read & 0x3FU) | 0x00;   /* DIO0 = 00 → RxDone */
    LoRa_write(_LoRa, REG_DIO_MAPPING_1, data);

    /* 14. Default private sync word (can be overwritten later) */
    LoRa_setSyncWord(_LoRa, 0x12);

    /* 15. Standby */
    LoRa_gotoMode(_LoRa, LORA_MODE_STANDBY);
    HAL_Delay(10);

    /* 16. Check silicon version (SX127x returns 0x12) */
    read = LoRa_read(_LoRa, REG_VERSION);
    if (read == 0x12) {
        return LORA_OK;
    }

    return LORA_NOT_FOUND;
}
