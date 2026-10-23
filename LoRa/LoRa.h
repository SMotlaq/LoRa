/**
 * @file LoRa.h
 * @brief Ra-02 / SX1278 LoRa library for STM32 (HAL drivers)
 *
 * This library provides a simple and reliable interface to control
 * Semtech SX1276/77/78/79 based modules (e.g. Ra-02) using STM32 HAL.
 *
 * Features:
 *  - Full control of frequency, SF, BW, CR, power, OCP, preamble
 *  - Transmit and Continuous/Single receive modes
 *  - RSSI and SNR measurement
 *  - Low Data Rate Optimization (automatic)
 *  - Sleep / Standby / TX / RX modes
 *  - Basic interrupt support via DIO0
 *
 * @author  Original: Salman Ami Motlaq (@S_Motlaq)
 * @author  Refactored & documented: 2026
 * @version 2.0.0
 * @date    2026
 *
 * @copyright MIT License
 *
 * @note Compatible with STM32CubeHAL. Tested on F0, F1, F4 and L4 series.
 */

#ifndef LORA_H
#define LORA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"   /* STM32 HAL + GPIO / SPI handles */

/* -------------------------------------------------------------------------- */
/*                              Timeouts                                      */
/* -------------------------------------------------------------------------- */
#define LORA_TRANSMIT_TIMEOUT_MS    2000U
#define LORA_RECEIVE_TIMEOUT_MS     2000U

/* -------------------------------------------------------------------------- */
/*                           Operating Modes                                  */
/* -------------------------------------------------------------------------- */
#define LORA_MODE_SLEEP             0x00
#define LORA_MODE_STANDBY           0x01
#define LORA_MODE_TRANSMIT          0x03
#define LORA_MODE_RX_CONTINUOUS     0x05
#define LORA_MODE_RX_SINGLE         0x06

/* -------------------------------------------------------------------------- */
/*                              Bandwidth                                     */
/* -------------------------------------------------------------------------- */
#define LORA_BW_7_8_KHZ             0
#define LORA_BW_10_4_KHZ            1
#define LORA_BW_15_6_KHZ            2
#define LORA_BW_20_8_KHZ            3
#define LORA_BW_31_25_KHZ           4
#define LORA_BW_41_7_KHZ            5
#define LORA_BW_62_5_KHZ            6
#define LORA_BW_125_KHZ             7
#define LORA_BW_250_KHZ             8
#define LORA_BW_500_KHZ             9

/* -------------------------------------------------------------------------- */
/*                             Coding Rate                                    */
/* -------------------------------------------------------------------------- */
#define LORA_CR_4_5                 1
#define LORA_CR_4_6                 2
#define LORA_CR_4_7                 3
#define LORA_CR_4_8                 4

/* -------------------------------------------------------------------------- */
/*                          Spreading Factor                                  */
/* -------------------------------------------------------------------------- */
#define LORA_SF_6                   6
#define LORA_SF_7                   7
#define LORA_SF_8                   8
#define LORA_SF_9                   9
#define LORA_SF_10                  10
#define LORA_SF_11                  11
#define LORA_SF_12                  12

/* -------------------------------------------------------------------------- */
/*                            Output Power                                    */
/* -------------------------------------------------------------------------- */
#define LORA_POWER_11DBM            0xF6
#define LORA_POWER_14DBM            0xF9
#define LORA_POWER_17DBM            0xFC
#define LORA_POWER_20DBM            0xFF

/* -------------------------------------------------------------------------- */
/*                         SX127x Register Map                                */
/* -------------------------------------------------------------------------- */
#define REG_FIFO                    0x00
#define REG_OP_MODE                 0x01
#define REG_FRF_MSB                 0x06
#define REG_FRF_MID                 0x07
#define REG_FRF_LSB                 0x08
#define REG_PA_CONFIG               0x09
#define REG_OCP                     0x0B
#define REG_LNA                     0x0C
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_FIFO_TX_BASE_ADDR       0x0E
#define REG_FIFO_RX_BASE_ADDR       0x0F
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_RX_NB_BYTES             0x13
#define REG_PKT_SNR_VALUE           0x19
#define REG_PKT_RSSI_VALUE          0x1A
#define REG_MODEM_CONFIG_1          0x1D
#define REG_MODEM_CONFIG_2          0x1E
#define REG_SYMB_TIMEOUT_LSB        0x1F
#define REG_PREAMBLE_MSB            0x20
#define REG_PREAMBLE_LSB            0x21
#define REG_PAYLOAD_LENGTH          0x22
#define REG_MODEM_CONFIG_3          0x26
#define REG_SYNC_WORD               0x39
#define REG_DIO_MAPPING_1           0x40
#define REG_VERSION                 0x42
#define REG_PA_DAC                  0x4D

/* -------------------------------------------------------------------------- */
/*                            Status Codes                                    */
/* -------------------------------------------------------------------------- */
#define LORA_OK                     200
#define LORA_NOT_FOUND              404
#define LORA_LARGE_PAYLOAD          413
#define LORA_UNAVAILABLE            503

/* -------------------------------------------------------------------------- */
/*                          LoRa Configuration Structure                      */
/* -------------------------------------------------------------------------- */

/**
 * @struct LoRa
 * @brief Main configuration and runtime structure for a LoRa module instance.
 */
typedef struct {
    /* ---- Hardware pins & SPI ------------------------------------------- */
    GPIO_TypeDef*       CS_port;            /**< NSS / CS GPIO port */
    uint16_t            CS_pin;             /**< NSS / CS GPIO pin */
    GPIO_TypeDef*       reset_port;         /**< RESET GPIO port */
    uint16_t            reset_pin;          /**< RESET GPIO pin */
    GPIO_TypeDef*       DIO0_port;          /**< DIO0 GPIO port (interrupt) */
    uint16_t            DIO0_pin;           /**< DIO0 GPIO pin */
    SPI_HandleTypeDef*  hSPIx;              /**< Pointer to SPI handle */

    /* ---- Radio parameters ---------------------------------------------- */
    int                 current_mode;       /**< Current operating mode */
    int                 frequency;          /**< Carrier frequency in MHz (e.g. 433) */
    uint8_t             spreadingFactor;    /**< SF6 … SF12 */
    uint8_t             bandwidth;          /**< One of LORA_BW_xxx */
    uint8_t             codingRate;         /**< One of LORA_CR_4_x */
    uint16_t            preamble;           /**< Preamble length (symbols) */
    uint8_t             power;              /**< One of LORA_POWER_xxDBM */
    uint8_t             overCurrentProtection; /**< OCP threshold in mA (45–240) */
} LoRa;

/* -------------------------------------------------------------------------- */
/*                         Public Function Prototypes                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Create a new LoRa instance with safe default values.
 * @return Initialized LoRa structure (frequency=433, SF7, BW125, CR4/5, +20 dBm)
 */
LoRa newLoRa(void);

/**
 * @brief Hardware reset of the module (via RESET pin).
 * @param[in] _LoRa Pointer to LoRa instance
 */
void LoRa_reset(LoRa* _LoRa);

/**
 * @brief Low-level register read (single or burst).
 */
void LoRa_readReg(LoRa* _LoRa, uint8_t* address, uint16_t addr_len,
                  uint8_t* output, uint16_t read_len);

/**
 * @brief Low-level register write (single or burst).
 */
void LoRa_writeReg(LoRa* _LoRa, uint8_t* address, uint16_t addr_len,
                   uint8_t* values, uint16_t write_len);

/**
 * @brief Change operating mode (Sleep / Standby / TX / RX Continuous / RX Single).
 * @param[in] _LoRa Pointer to LoRa instance
 * @param[in] mode  One of LORA_MODE_xxx
 */
void LoRa_gotoMode(LoRa* _LoRa, int mode);

/**
 * @brief Read a single register.
 * @return Register value
 */
uint8_t LoRa_read(LoRa* _LoRa, uint8_t address);

/**
 * @brief Write a single register.
 */
void LoRa_write(LoRa* _LoRa, uint8_t address, uint8_t value);

/**
 * @brief Burst write to FIFO (used for TX payload).
 */
void LoRa_BurstWrite(LoRa* _LoRa, uint8_t address, uint8_t* value, uint8_t length);

/**
 * @brief Check if the LoRa structure has valid hardware pointers.
 * @return 1 if valid, 0 otherwise
 */
uint8_t LoRa_isvalid(LoRa* _LoRa);

/**
 * @brief Enable / disable Low Data Rate Optimization manually.
 * @param[in] value 0 = disable, non-zero = enable
 */
void LoRa_setLowDataRateOptimization(LoRa* _LoRa, uint8_t value);

/**
 * @brief Automatically enable LDO when symbol duration > 16 ms.
 */
void LoRa_setAutoLDO(LoRa* _LoRa);

/**
 * @brief Set carrier frequency.
 * @param[in] freq Frequency in MHz (integer, e.g. 433 or 434)
 */
void LoRa_setFrequency(LoRa* _LoRa, int freq);

/**
 * @brief Set Spreading Factor (SF6 … SF12).
 */
void LoRa_setSpreadingFactor(LoRa* _LoRa, int sf);

/**
 * @brief Set output power.
 * @param[in] power One of LORA_POWER_11DBM … LORA_POWER_20DBM
 */
void LoRa_setPower(LoRa* _LoRa, uint8_t power);

/**
 * @brief Set Over Current Protection threshold.
 * @param[in] current Desired limit in mA (45 … 240)
 */
void LoRa_setOCP(LoRa* _LoRa, uint8_t current);

/**
 * @brief Set RX symbol timeout MSB and enable CRC.
 */
void LoRa_setTOMsb_setCRCon(LoRa* _LoRa);

/**
 * @brief Set Sync Word (default 0x12 for private networks).
 */
void LoRa_setSyncWord(LoRa* _LoRa, uint8_t syncword);

/**
 * @brief Transmit a packet (blocking with timeout).
 * @param[in] data    Payload buffer
 * @param[in] length  Payload length (1 … 255)
 * @param[in] timeout Timeout in milliseconds
 * @return 1 on success (TxDone), 0 on timeout
 */
uint8_t LoRa_transmit(LoRa* _LoRa, uint8_t* data, uint8_t length, uint16_t timeout);

/**
 * @brief Start continuous receive mode.
 */
void LoRa_startReceiving(LoRa* _LoRa);

/**
 * @brief Read the last received packet (polling).
 * @param[out] data   Buffer to store payload
 * @param[in]  length Maximum bytes to read
 * @return Number of bytes actually received (0 if none)
 */
uint8_t LoRa_receive(LoRa* _LoRa, uint8_t* data, uint8_t length);

/**
 * @brief Interrupt-friendly receive (call from DIO0 EXTI callback).
 * @param[out] data   Buffer to store payload
 * @param[in]  length Maximum bytes to read
 * @return Number of bytes received
 */
uint8_t LoRa_receive_IT(LoRa* _LoRa, uint8_t* data, uint8_t length);

/**
 * @brief Get Packet RSSI of the last received packet (dBm).
 * @return RSSI in dBm (approximately)
 */
int LoRa_getRSSI(LoRa* _LoRa);

/**
 * @brief Get Packet SNR of the last received packet (dB).
 * @return SNR in dB (resolution 0.25 dB)
 */
float LoRa_getSNR(LoRa* _LoRa);

/**
 * @brief Initialize the module with all parameters stored in the structure.
 * @return LORA_OK, LORA_NOT_FOUND or LORA_UNAVAILABLE
 */
uint16_t LoRa_init(LoRa* _LoRa);

#ifdef __cplusplus
}
#endif

#endif /* LORA_H */
