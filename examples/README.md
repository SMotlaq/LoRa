# LoRa Library Examples

This folder contains ready-to-use example codes for the STM32 LoRa library.

## Available Examples

| File                        | Description                                      | Difficulty |
|-----------------------------|--------------------------------------------------|------------|
| `01_basic_tx.c`             | Simple periodic transmit                         | Beginner   |
| `02_basic_rx_polling.c`     | Receive using polling                            | Beginner   |
| `03_basic_rx_interrupt.c`   | Receive using DIO0 interrupt (recommended)       | Beginner   |
| `04_tx_rx_pingpong.c`       | Bidirectional Ping-Pong communication            | Intermediate |
| `05_long_range_config.c`    | Maximum range configuration (SF12 + high power)  | Intermediate |

## How to Use

1. Copy the desired example function into your `main.c` (or keep the file and call the function).
2. Change the pin definitions and SPI handle at the top of the file to match your hardware.
3. Make sure SPI and the required GPIOs are initialized (CubeMX recommended).
4. For interrupt examples, configure DIO0 as external interrupt (rising edge) and call the provided IRQ handler from `HAL_GPIO_EXTI_Callback`.

## Recommended First Test

1. Flash `01_basic_tx.c` on Board A.
2. Flash `03_basic_rx_interrupt.c` on Board B.
3. Open a serial terminal on Board B to see received packets (add UART prints if needed).

## Notes

- All examples use **433 MHz** by default. Change `myLoRa.frequency` if you use a different band.
- Always connect an antenna before transmitting.
- Power the module with **3.3 V** only.
- When using `LORA_POWER_20DBM`, set OCP to at least 140 mA.

Happy testing! 📡