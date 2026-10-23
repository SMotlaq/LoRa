/**
 * @file 03_basic_rx_interrupt.c
 * @brief LoRa Receive example using DIO0 interrupt (recommended)
 *
 * Configure DIO0 as external interrupt (rising edge) in CubeMX.
 * This is the preferred way to receive packets.
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

/* Flag set from the interrupt */
volatile uint8_t packet_ready = 0;

void Example_Basic_RX_Interrupt(void)
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

    /* Enter continuous receive mode */
    LoRa_startReceiving(&myLoRa);

    while (1) {
        if (packet_ready) {
            packet_ready = 0;

            len = LoRa_receive_IT(&myLoRa, buffer, sizeof(buffer));

            if (len > 0) {
                rssi = LoRa_getRSSI(&myLoRa);
                snr  = LoRa_getSNR(&myLoRa);

                /* Process the packet here */
                /* e.g. send over UART, parse data, etc. */
            }
        }

        /* Do other tasks here */
        HAL_Delay(1);
    }
}

/**
 * @brief This function must be called from your EXTI callback.
 *
 * Example (in stm32xxxx_it.c or main.c):
 *
 * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * {
 *     if (GPIO_Pin == DIO0_Pin) {
 *         Example_DIO0_IRQHandler();
 *     }
 * }
 */
void Example_DIO0_IRQHandler(void)
{
    packet_ready = 1;
}