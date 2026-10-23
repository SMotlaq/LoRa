/**
 * @file 01_basic_tx.c
 * @brief Simple LoRa Transmit example
 *
 * Sends a "Hello LoRa!" message every 2 seconds.
 *
 * Hardware:
 *   - Ra-02 (SX1278) connected via SPI
 *   - NSS, RESET and DIO0 pins configured in CubeMX / your project
 *
 * Before running:
 *   1. Initialize SPI and GPIOs in your project
 *   2. Set the correct pin names below
 *   3. Call this code after HAL_Init() and peripheral init
 */

#include "main.h"
#include "LoRa.h"
#include <string.h>
#include <stdio.h>

/* ============== User Configuration ============== */
extern SPI_HandleTypeDef hspi1;          /* Change to your SPI handle */

#define NSS_GPIO_Port    GPIOA
#define NSS_Pin          GPIO_PIN_4
#define RESET_GPIO_Port  GPIOA
#define RESET_Pin        GPIO_PIN_0
#define DIO0_GPIO_Port   GPIOA
#define DIO0_Pin         GPIO_PIN_1
/* ================================================ */

LoRa myLoRa;

void Example_Basic_TX(void)
{
    uint8_t message[] = "Hello LoRa!";
    uint8_t status;

    /* Create and configure LoRa instance */
    myLoRa = newLoRa();

    myLoRa.hSPIx      = &hspi1;
    myLoRa.CS_port    = NSS_GPIO_Port;
    myLoRa.CS_pin     = NSS_Pin;
    myLoRa.reset_port = RESET_GPIO_Port;
    myLoRa.reset_pin  = RESET_Pin;
    myLoRa.DIO0_port  = DIO0_GPIO_Port;
    myLoRa.DIO0_pin   = DIO0_Pin;

    /* Optional radio settings */
    myLoRa.frequency       = 433;
    myLoRa.spreadingFactor = LORA_SF_7;
    myLoRa.bandwidth       = LORA_BW_125_KHZ;
    myLoRa.codingRate      = LORA_CR_4_5;
    myLoRa.power           = LORA_POWER_17DBM;
    myLoRa.preamble        = 8;

    /* Initialize module */
    if (LoRa_init(&myLoRa) != LORA_OK) {
        /* Initialization failed - check wiring and SPI */
        while (1) {
            /* Error handler - blink LED or print error */
        }
    }

    /* Main transmit loop */
    while (1) {
        status = LoRa_transmit(&myLoRa, message, strlen((char*)message), 1000);

        if (status) {
            /* Transmit successful */
        } else {
            /* Timeout occurred */
        }

        HAL_Delay(2000);   /* Send every 2 seconds */
    }
}