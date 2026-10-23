<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

# LoRa Library for STM32 (SX1278 / Ra-02)

A clean, well-documented, and reliable HAL-based library for the **Ra-02** (and other SX1276/77/78/79 based) LoRa modules on STM32 microcontrollers.

> **Version 2.0 - Oct 2026** – Major cleanup, bug fixes, SNR support, improved RSSI, Doxygen documentation, and compatibility layer for older code.

---

## Donate

Is it helpful?

<p align="left">
  <a href="http://smotlaq.ir/LQgQF">
  <img src="https://raw.githubusercontent.com/SMotlaq/LoRa/master/bmc.png" width="200" alt="Buy me a Coffee"/>
  </a>
</p>

---

## Tutorial Video

<p align="left">
  <a href="http://smotlaq.ir/BCrSo">
  <img src="https://raw.githubusercontent.com/SMotlaq/LoRa/master/tut.jpg" alt="YouTube video"/>
  </a>
</p>

---

## Features

- Full control over LoRa parameters (Frequency, Spreading Factor, Bandwidth, Coding Rate, Power, Preamble, OCP)
- Blocking transmit with timeout
- Continuous receive mode + interrupt-friendly receive (`LoRa_receive_IT`)
- RSSI and **SNR** measurement
- Automatic Low Data Rate Optimization
- Sleep / Standby / TX / RX modes
- Hardware reset support
- Clean Doxygen documentation
- Compatibility layer for code written with the old API (before October 2026)

---

## Successfully Tested On

| MCU              | Board                  | Status |
|------------------|------------------------|--------|
| STM32F103C8      | Blue Pill              | ✅     |
| STM32F103RE      |                        | ✅     |
| STM32F030C8      |                        | ✅     |
| STM32F411VE      | Discovery              | ✅     |
| STM32F429ZG      |                        | ✅     |
| STM32L476RG      | Nucleo-L476RG          | ✅     |

---

## Hardware Requirements

- Ra-02 LoRa module (SX1278) or compatible SX127x module
- 433 MHz antenna (or matching antenna for your frequency)
- STM32 microcontroller with SPI

### Wiring

| STM32 Pin                          | Ra-02 Pin |
|------------------------------------|-----------|
| SPI MISO                           | MISO      |
| SPI MOSI                           | MOSI      |
| SPI SCK                            | SCK       |
| Any GPIO (output, default HIGH)    | NSS       |
| Any GPIO (output, default HIGH)    | RST       |
| Any GPIO (input + EXTI rising)     | DIO0      |
| 3.3 V                              | 3.3 V     |
| GND                                | GND       |

> **Important:** Never power the module with 5 V. Always use 3.3 V and connect an antenna before transmitting.

---

## Installation

1. Copy `LoRa.h` and `LoRa.c` into your project.
2. Add the include path if necessary.
3. Include the header in your source files:

```c
#include "LoRa.h"
```

### Using the Compatibility Layer (for old code)

If you have existing projects written with the **old API** (before October 2026 / version < 2.0), also include the compatibility header:

```c
#include "LoRa.h"
#include "LoRa_compat.h"   // ← enables old names
```

This restores most of the old `#define`s (`SF_7`, `BW_125KHz`, `STNBY_MODE`, `POWER_20db`, etc.).

**Note:** You still need to rename the structure members (see [Migration Guide](#migration-from-old-versions-before-october-2026)).

---

## Quick Start

```c
#include "LoRa.h"

LoRa myLoRa;

int main(void)
{
    // ... HAL init, SPI init, GPIO init ...

    myLoRa = newLoRa();

    // Hardware pins
    myLoRa.CS_port     = NSS_GPIO_Port;
    myLoRa.CS_pin      = NSS_Pin;
    myLoRa.reset_port  = RESET_GPIO_Port;
    myLoRa.reset_pin   = RESET_Pin;
    myLoRa.DIO0_port   = DIO0_GPIO_Port;
    myLoRa.DIO0_pin    = DIO0_Pin;
    myLoRa.hSPIx       = &hspi1;

    // Optional radio settings (defaults are fine for many cases)
    myLoRa.frequency        = 433;                 // MHz
    myLoRa.spreadingFactor  = LORA_SF_7;
    myLoRa.bandwidth        = LORA_BW_125_KHZ;
    myLoRa.codingRate       = LORA_CR_4_5;
    myLoRa.power            = LORA_POWER_17DBM;
    myLoRa.overCurrentProtection = 100;            // mA
    myLoRa.preamble         = 8;

    uint16_t status = LoRa_init(&myLoRa);

    if (status == LORA_OK) {
        // Module is ready
    } else if (status == LORA_NOT_FOUND) {
        // Cannot communicate with the module (check wiring / SPI)
    } else {
        // Invalid configuration (NULL pointers)
    }

    while (1) {
        // Your application
    }
}
```

---

## Configuration Options

### Spreading Factor
```c
LORA_SF_6 ... LORA_SF_12
```

### Bandwidth
```c
LORA_BW_7_8_KHZ, LORA_BW_10_4_KHZ, LORA_BW_15_6_KHZ,
LORA_BW_20_8_KHZ, LORA_BW_31_25_KHZ, LORA_BW_41_7_KHZ,
LORA_BW_62_5_KHZ, LORA_BW_125_KHZ, LORA_BW_250_KHZ, LORA_BW_500_KHZ
```

### Coding Rate
```c
LORA_CR_4_5, LORA_CR_4_6, LORA_CR_4_7, LORA_CR_4_8
```

### Output Power
```c
LORA_POWER_11DBM, LORA_POWER_14DBM, LORA_POWER_17DBM, LORA_POWER_20DBM
```

> When using `LORA_POWER_20DBM`, the library automatically enables high-power mode via `RegPaDac`.  
> Make sure your OCP setting is high enough (≥ 140 mA recommended for +20 dBm).

### Over Current Protection
Valid range: **45 mA – 240 mA**  
(Values are rounded according to the datasheet formula.)

---

## Transmitting Data

```c
uint8_t data[] = "Hello LoRa!";
uint8_t success = LoRa_transmit(&myLoRa, data, sizeof(data) - 1, 1000); // 1 second timeout

if (success) {
    // Transmission completed (TxDone)
} else {
    // Timeout
}
```

**Note:** After calling `LoRa_transmit()`, the FIFO is overwritten. If you need to receive right after transmitting, add a short delay or switch back to receive mode explicitly.

---

## Receiving Data

### Polling mode

```c
LoRa_startReceiving(&myLoRa);

uint8_t buffer[64];
uint8_t len = LoRa_receive(&myLoRa, buffer, sizeof(buffer));

if (len > 0) {
    // New packet received
    int rssi = LoRa_getRSSI(&myLoRa);
    float snr = LoRa_getSNR(&myLoRa);
}
```

### Interrupt mode (recommended)

1. Configure DIO0 as external interrupt (rising edge).
2. In the EXTI callback:

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DIO0_Pin) {
        uint8_t buffer[64];
        uint8_t len = LoRa_receive_IT(&myLoRa, buffer, sizeof(buffer));

        if (len > 0) {
            // Process packet
            int rssi = LoRa_getRSSI(&myLoRa);
            float snr = LoRa_getSNR(&myLoRa);
        }
    }
}
```

---

## Signal Quality

```c
int   rssi = LoRa_getRSSI(&myLoRa);   // dBm (improved, uses SNR when negative)
float snr  = LoRa_getSNR(&myLoRa);    // dB  (resolution 0.25 dB)
```

---

## Migration from Old Versions (before October 2026)

The October 2026 release (v2.0) introduced several breaking changes for better code quality:

| Old name              | New name                  |
|-----------------------|---------------------------|
| `spredingFactor`      | `spreadingFactor`         |
| `bandWidth`           | `bandwidth`               |
| `crcRate`             | `codingRate`              |
| `STNBY_MODE`          | `LORA_MODE_STANDBY`       |
| `POWER_20db`          | `LORA_POWER_20DBM`        |
| `SF_7`, `BW_125KHz`…  | `LORA_SF_7`, `LORA_BW_125_KHZ`… |

### Recommended migration steps

1. Replace `LoRa.h` / `LoRa.c` with the new files.
2. Add `#include "LoRa_compat.h"` temporarily.
3. Rename the first three structure members listed above.
4. (Optional) Gradually switch to the new `LORA_` prefixed defines.
5. Remove `LoRa_compat.h` when your code is fully updated.

A detailed migration guide is available in [`MIGRATION.md`](MIGRATION.md).

---

## Status Codes

| Code | Macro              | Meaning                                      |
|------|--------------------|----------------------------------------------|
| 200  | `LORA_OK`          | Initialization successful                    |
| 404  | `LORA_NOT_FOUND`   | Module not responding (SPI / wiring issue)   |
| 413  | `LORA_LARGE_PAYLOAD` | Payload larger than 255 bytes              |
| 503  | `LORA_UNAVAILABLE` | Invalid configuration (NULL pointers etc.)   |

---

## Project Structure (Recommended)

```
YourProject/
├── Core/
│   └── ...
├── Drivers/
│   └── ...
├── LoRa/
│   ├── LoRa.h
│   ├── LoRa.c
│   └── LoRa_compat.h      ← only if you need old API
└── ...
```

---

## Documentation

All public functions are documented with Doxygen comments.  
You can generate HTML documentation with:

```bash
doxygen Doxyfile
```

---

## Contributing

Contributions are welcome! Please:

1. Open an issue first for major changes.
2. Follow the existing code style.
3. Keep the compatibility layer working when possible.
4. Update documentation if needed.

---

## Author & Contact

**Original author:** Salman Ami Motlaq ([@S_Motlaq](https://t.me/s_motlaq))  
Email: pilot.motlaq@gmail.com

If this library saved you time, consider giving it a ⭐ on GitHub!

<!-- MARKDOWN LINKS & IMAGES -->
[contributors-shield]: https://img.shields.io/github/contributors/SMotlaq/LoRa.svg?style=for-the-badge
[contributors-url]: https://github.com/SMotlaq/LoRa/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/SMotlaq/LoRa.svg?style=for-the-badge
[forks-url]: https://github.com/SMotlaq/LoRa/network/members
[stars-shield]: https://img.shields.io/github/stars/SMotlaq/LoRa.svg?style=for-the-badge
[stars-url]: https://github.com/SMotlaq/LoRa/stargazers
[issues-shield]: https://img.shields.io/github/issues/SMotlaq/LoRa.svg?style=for-the-badge
[issues-url]: https://github.com/SMotlaq/LoRa/issues
[license-shield]: https://img.shields.io/github/license/SMotlaq/LoRa.svg?style=for-the-badge
[license-url]: https://github.com/SMotlaq/LoRa/blob/master/LICENSE
