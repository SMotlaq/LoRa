/**
 * @file 02_basic_rx_polling.c
 * @brief Simple LoRa Receive example (Polling mode)
 *
 * Continuously listens for incoming packets and reads them.
 *
 * Note: Polling is simple but less efficient.
 *       Prefer the interrupt version (03_basic_rx_interrupt.c) for real applications.
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

void Example_Basic_RX_Polling(void)
{
    uint8_t buffer[64];
    uint8_t len;
    int rssi;
    float snr;

    myLoRa = newLoRa();

    myLoRa.hSPIx      = &hspi1;
    myLoRa.CS_port    = NSS_GPIO_Port;
    myLoRa.CS_pin     = NSS_Pin;
    myLoRa.reset_port = RESET_GPIO_Port;
    myLoRa.reset_pin  = RESET_Pin;
    myLoRa.DIO0_port  = DIO0_GPIO_Port;
    myLoRa.DIO0_pin   = DIO0_Pin;

    myLoRa.frequency       = 433;
    myLoRa.spreadingFactor = LORA_SF_7;
    myLoRa.bandwidth       = LORA_BW_125_KHZ;
    myLoRa.codingRate      = LORA_CR_4_5;

    if (LoRa_init(&myLoRa) != LORA_OK) {
        while (1) { /* Error */ }
    }

    /* Start continuous receive mode */
    LoRa_startReceiving(&myLoRa);

    while (1) {
        len = LoRa_receive(&myLoRa, buffer, sizeof(buffer));

        if (len > 0) {
            rssi = LoRa_getRSSI(&myLoRa);
            snr  = LoRa_getSNR(&myLoRa);

            /* Packet received.
             * You can print it via UART, for example:
             *
             * char msg[128];
             * snprintf(msg, sizeof(msg), "RX (%d bytes) RSSI=%d SNR=%.2f: %.*s\r\n",
             *          len, rssi, snr, len, buffer);
             * HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
             */
        }

        HAL_Delay(10);   /* Small delay to avoid busy loop */
    }
}