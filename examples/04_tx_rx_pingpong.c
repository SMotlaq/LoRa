/**
 * @file 04_tx_rx_pingpong.c
 * @brief Simple Ping-Pong example (Transmit + Receive)
 *
 * One board sends a packet, the other replies.
 * Useful for range testing and basic bidirectional communication.
 *
 * Both boards can run the same code.
 * The board that starts first will be the first transmitter.
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

void Example_PingPong(void)
{
    uint8_t tx_buffer[32];
    uint8_t rx_buffer[32];
    uint8_t len;
    uint32_t counter = 0;
    uint32_t last_tx_time = 0;

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
    myLoRa.power           = LORA_POWER_14DBM;

    if (LoRa_init(&myLoRa) != LORA_OK) {
        while (1) { /* Error */ }
    }

    LoRa_startReceiving(&myLoRa);

    while (1) {
        /* Check for incoming packet */
        len = LoRa_receive(&myLoRa, rx_buffer, sizeof(rx_buffer));

        if (len > 0) {
            /* Received something → reply after a short delay */
            HAL_Delay(50);

            snprintf((char*)tx_buffer, sizeof(tx_buffer), "Pong %lu", counter++);
            LoRa_transmit(&myLoRa, tx_buffer, strlen((char*)tx_buffer), 1000);

            /* Go back to receive mode */
            LoRa_startReceiving(&myLoRa);
            last_tx_time = HAL_GetTick();
        }

        /* If nothing received for a while, send a Ping */
        if (HAL_GetTick() - last_tx_time > 3000) {
            snprintf((char*)tx_buffer, sizeof(tx_buffer), "Ping %lu", counter++);
            LoRa_transmit(&myLoRa, tx_buffer, strlen((char*)tx_buffer), 1000);

            LoRa_startReceiving(&myLoRa);
            last_tx_time = HAL_GetTick();
        }

        HAL_Delay(10);
    }
}