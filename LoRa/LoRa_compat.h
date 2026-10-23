/**
 * @file LoRa_compat.h
 * @brief Compatibility layer for the old SMotlaq/LoRa API
 *
 * Include this header AFTER including the new LoRa.h if you want
 * to keep using the old names (spredingFactor, STNBY_MODE, etc.)
 * without changing your existing application code.
 *
 * Example:
 *   #include "LoRa.h"
 *   #include "LoRa_compat.h"   // <-- add this line
 *
 * @note This is a temporary migration aid. It is recommended to
 *       eventually update your code to the new clean API.
 */

#ifndef LORA_COMPAT_H
#define LORA_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*                         Old → New name mapping                             */
/* -------------------------------------------------------------------------- */

/* Operating Modes */
#define SLEEP_MODE              LORA_MODE_SLEEP
#define STNBY_MODE              LORA_MODE_STANDBY
#define TRANSMIT_MODE           LORA_MODE_TRANSMIT
#define RXCONTIN_MODE           LORA_MODE_RX_CONTINUOUS
#define RXSINGLE_MODE           LORA_MODE_RX_SINGLE

/* Bandwidth */
#define BW_7_8KHz               LORA_BW_7_8_KHZ
#define BW_10_4KHz              LORA_BW_10_4_KHZ
#define BW_15_6KHz              LORA_BW_15_6_KHZ
#define BW_20_8KHz              LORA_BW_20_8_KHZ
#define BW_31_25KHz             LORA_BW_31_25_KHZ
#define BW_41_7KHz              LORA_BW_41_7_KHZ
#define BW_62_5KHz              LORA_BW_62_5_KHZ
#define BW_125KHz               LORA_BW_125_KHZ
#define BW_250KHz               LORA_BW_250_KHZ
#define BW_500KHz               LORA_BW_500_KHZ

/* Coding Rate */
#define CR_4_5                  LORA_CR_4_5
#define CR_4_6                  LORA_CR_4_6
#define CR_4_7                  LORA_CR_4_7
#define CR_4_8                  LORA_CR_4_8

/* Spreading Factor */
#define SF_6                    LORA_SF_6
#define SF_7                    LORA_SF_7
#define SF_8                    LORA_SF_8
#define SF_9                    LORA_SF_9
#define SF_10                   LORA_SF_10
#define SF_11                   LORA_SF_11
#define SF_12                   LORA_SF_12

/* Power */
#define POWER_11db              LORA_POWER_11DBM
#define POWER_14db              LORA_POWER_14DBM
#define POWER_17db              LORA_POWER_17DBM
#define POWER_20db              LORA_POWER_20DBM

/* Old register names (if someone used them directly) */
#define RegFiFo                 REG_FIFO
#define RegOpMode               REG_OP_MODE
#define RegFrMsb                REG_FRF_MSB
#define RegFrMid                REG_FRF_MID
#define RegFrLsb                REG_FRF_LSB
#define RegPaConfig             REG_PA_CONFIG
#define RegOcp                  REG_OCP
#define RegLna                  REG_LNA
#define RegFiFoAddPtr           REG_FIFO_ADDR_PTR
#define RegFiFoTxBaseAddr       REG_FIFO_TX_BASE_ADDR
#define RegFiFoRxBaseAddr       REG_FIFO_RX_BASE_ADDR
#define RegFiFoRxCurrentAddr    REG_FIFO_RX_CURRENT_ADDR
#define RegIrqFlags             REG_IRQ_FLAGS
#define RegRxNbBytes            REG_RX_NB_BYTES
#define RegPktRssiValue         REG_PKT_RSSI_VALUE
#define RegPktSnrValue          REG_PKT_SNR_VALUE
#define RegModemConfig1         REG_MODEM_CONFIG_1
#define RegModemConfig2         REG_MODEM_CONFIG_2
#define RegSymbTimeoutL         REG_SYMB_TIMEOUT_LSB
#define RegPreambleMsb          REG_PREAMBLE_MSB
#define RegPreambleLsb          REG_PREAMBLE_LSB
#define RegPayloadLength        REG_PAYLOAD_LENGTH
#define RegModemConfig3         REG_MODEM_CONFIG_3
#define RegSyncWord             REG_SYNC_WORD
#define RegDioMapping1          REG_DIO_MAPPING_1
#define RegVersion              REG_VERSION
#define RegPaDac                REG_PA_DAC

/* -------------------------------------------------------------------------- */
/*                    Structure member name aliases                           */
/* -------------------------------------------------------------------------- */

/*
 * Note: C does not support real member aliases.
 * The cleanest way is to tell users to replace:
 *
 *   myLoRa.spredingFactor   →  myLoRa.spreadingFactor
 *   myLoRa.bandWidth        →  myLoRa.bandwidth
 *   myLoRa.crcRate          →  myLoRa.codingRate
 *
 * Below we provide helper macros that can be used during migration.
 */

/* Helper macros for setting the renamed fields (optional) */
#define LoRa_setSpredingFactor(lora_ptr, value) \
    do { (lora_ptr)->spreadingFactor = (value); } while(0)

#define LoRa_setBandWidth(lora_ptr, value) \
    do { (lora_ptr)->bandwidth = (value); } while(0)

#define LoRa_setCrcRate(lora_ptr, value) \
    do { (lora_ptr)->codingRate = (value); } while(0)

/* -------------------------------------------------------------------------- */
/*                    Old function name aliases                               */
/* -------------------------------------------------------------------------- */

/* These functions kept the same name, so no alias needed:
 *   newLoRa, LoRa_reset, LoRa_readReg, LoRa_writeReg,
 *   LoRa_gotoMode, LoRa_read, LoRa_write, LoRa_BurstWrite,
 *   LoRa_isvalid, LoRa_setFrequency, LoRa_setSpreadingFactor,
 *   LoRa_setPower, LoRa_setOCP, LoRa_setTOMsb_setCRCon,
 *   LoRa_setSyncWord, LoRa_transmit, LoRa_startReceiving,
 *   LoRa_receive, LoRa_receive_IT, LoRa_getRSSI, LoRa_init
 */

/* Old misspelled function */
#define LoRa_setLowDaraRateOptimization   LoRa_setLowDataRateOptimization

/* -------------------------------------------------------------------------- */
/*                    Timeout macros (old names)                              */
/* -------------------------------------------------------------------------- */
#define TRANSMIT_TIMEOUT        LORA_TRANSMIT_TIMEOUT_MS
#define RECEIVE_TIMEOUT         LORA_RECEIVE_TIMEOUT_MS

#ifdef __cplusplus
}
#endif

#endif /* LORA_COMPAT_H */
