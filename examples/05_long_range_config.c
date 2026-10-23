/**
 * @file 05_long_range_config.c
 * @brief Example of long-range configuration (SF12 + low bandwidth)
 *
 * This configuration prioritizes range over data rate.
 * Suitable for sensors that send small packets infrequently.
 *
 * Trade-offs:
 *   + Maximum range
 *   + Better sensitivity
 *   - Very low data rate
 *   - Long airtime (watch duty-cycle regulations!)
 */

#include "main.h"
#include "LoRa.h"
#include <string.h>
#include <stdio.h>

/* ============== User Configuration ============== */
extern SPI_HandleTypeDef hspi1;

#define NSS_GPIO_Port    GPIOA
#define NSS_Pin          GPIO_PIN_4
#define RESET_GPIO_Port  GPIOA
#define RESET_Pin        GPIO_PIN_0
#define DIO0_GPIO_Port   GPIOA
#define DIO0_Pin         GPIO_PIN_1
/* ================================================ */

LoRa myLoRa;

void Example_LongRange(void)
{
    uint8_t sensor_data[16];
    uint8_t len = 0;

    myLoRa = newLoRa();

    myLoRa.hSPIx      = &hspi1;
    myLoRa.CS_port    = NSS_GPIO_Port;
    myLoRa.CS_pin     = NSS_Pin;
    myLoRa.reset_port = RESET_GPIO_Port;
    myLoRa.reset_pin  = RESET_Pin;
    myLoRa.DIO0_port  = DIO0_GPIO_Port;
    myLoRa.DIO0_pin   = DIO0_Pin;

    /* Long-range settings */
    myLoRa.frequency       = 433;
    myLoRa.spreadingFactor = LORA_SF_12;       /* Maximum SF */
    myLoRa.bandwidth       = LORA_BW_125_KHZ;  /* or LORA_BW_62_5_KHZ for even more range */
    myLoRa.codingRate      = LORA_CR_4_8;      /* Highest coding rate */
    myLoRa.power           = LORA_POWER_20DBM; /* Maximum power */
    myLoRa.overCurrentProtection = 140;        /* Recommended for +20 dBm */
    myLoRa.preamble        = 12;

    if (LoRa_init(&myLoRa) != LORA_OK) {
        while (1) { /* Error */ }
    }

    /* Example: send a small payload every 60 seconds */
    while (1) {
        /* Prepare your sensor data here */
        snprintf((char*)sensor_data, sizeof(sensor_data), "T:25.3");

        LoRa_transmit(&myLoRa, sensor_data, strlen((char*)sensor_data), 5000);

        /* Important: with SF12 the airtime is long.
         * Respect local duty-cycle limits (e.g. 1% in EU868).
         */
        HAL_Delay(60000);   /* 60 seconds */
    }
}