# Migration Guide: Old API → New API (v2.0)

This document helps you move from the original SMotlaq/LoRa library to the cleaned-up version.

## Quick Compatibility (Minimal Changes)

1. Replace the old `LoRa.h` / `LoRa.c` with the new ones.
2. Add one extra include in your files:

```c
#include "LoRa.h"
#include "LoRa_compat.h"   // ← add this line
```

Most of the old `#define`s will continue to work thanks to the compatibility layer.

## Structure Member Names (Must Change)

These field names were fixed (typos corrected). You **must** update them in your code:

| Old name (wrong)     | New name (correct)    |
|----------------------|-----------------------|
| `spredingFactor`     | `spreadingFactor`     |
| `bandWidth`          | `bandwidth`           |
| `crcRate`            | `codingRate`          |

**Example:**

```c
// Old
myLoRa.spredingFactor = SF_9;
myLoRa.bandWidth      = BW_250KHz;
myLoRa.crcRate        = CR_4_8;

// New
myLoRa.spreadingFactor = LORA_SF_9;   // or SF_9 if using compat.h
myLoRa.bandwidth       = LORA_BW_250_KHZ;
myLoRa.codingRate      = LORA_CR_4_8;
```

## Recommended New Names (Optional but Cleaner)

| Old define          | Recommended new define     |
|---------------------|----------------------------|
| `SF_7` … `SF_12`    | `LORA_SF_7` … `LORA_SF_12` |
| `BW_125KHz` etc.    | `LORA_BW_125_KHZ` etc.     |
| `CR_4_5` etc.       | `LORA_CR_4_5` etc.         |
| `POWER_20db`        | `LORA_POWER_20DBM`         |
| `STNBY_MODE`        | `LORA_MODE_STANDBY`        |
| `SLEEP_MODE`        | `LORA_MODE_SLEEP`          |
| `TRANSMIT_MODE`     | `LORA_MODE_TRANSMIT`       |
| `RXCONTIN_MODE`     | `LORA_MODE_RX_CONTINUOUS`  |

## New Features Available

- `LoRa_getSNR()` – returns SNR in dB (float)
- Better `LoRa_getRSSI()` (takes SNR into account when negative)
- `LoRa_receive_IT()` is now fully implemented
- SF6 is supported
- Proper null checks and safer code

## Full Example After Migration

```c
#include "LoRa.h"
// #include "LoRa_compat.h"   // only if you still need old defines

LoRa myLoRa;

int main(void)
{
    myLoRa = newLoRa();

    myLoRa.CS_port         = NSS_GPIO_Port;
    myLoRa.CS_pin          = NSS_Pin;
    myLoRa.reset_port      = RESET_GPIO_Port;
    myLoRa.reset_pin       = RESET_Pin;
    myLoRa.DIO0_port       = DIO0_GPIO_Port;
    myLoRa.DIO0_pin        = DIO0_Pin;
    myLoRa.hSPIx           = &hspi1;

    // New clean names
    myLoRa.frequency       = 433;
    myLoRa.spreadingFactor = LORA_SF_9;
    myLoRa.bandwidth       = LORA_BW_125_KHZ;
    myLoRa.codingRate      = LORA_CR_4_5;
    myLoRa.power           = LORA_POWER_17DBM;
    myLoRa.overCurrentProtection = 100;
    myLoRa.preamble        = 8;

    if (LoRa_init(&myLoRa) == LORA_OK) {
        // success
    }

    // ...
}
```

## Need Help?

Open an issue or contact the maintainer.
