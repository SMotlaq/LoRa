# LoRa module (SX1278) library for STM32 and HAL

## Installation
From the Src folder, clone the LoRa.c file and from Inc folder, clone LoRa.h file.
then import them into your main.c:
```bash
#include "LoRa.h"
```
## Usage
### Initial configuration
First of all, you have to create a LoRa object globaly:
```bash
/* USER CODE BEGIN PV */
 LoRa myLoRa;
/* USER CODE END PV */
```
Then call its constructor in the main():
```bash
 myLoRa = newLoRa();
```
Now, the default setting is set in myLoRa, but you should determine 5 important parameters:

1- NSS pin and port

2- Reset pin and port

3- The SPI that you used for communication

```bash
myLoRa.CS_port         = NSS_GPIO_Port;
myLoRa.CS_pin          = NSS_Pin;
myLoRa.reset_port      = RESET_GPIO_Port;
myLoRa.reset_pin       = RESET_Pin;
myLoRa.hSPIx           = &hspi3;
```
Other parameters are set like this:
```bash
myLoRa.frequency             = 434;             // default = 433 MHz
myLoRa.spredingFactor        = SF_9;            // default = SF_7
myLoRa.bandWidth             = BW_250KHz;       // default = BW_125KHz
myLoRa.crcRate               = CR_4_8;          // default = CR_4_5
myLoRa.power                 = POWER_17db;      // default = 20db
myLoRa.overCurrentProtection = 130;             // default = 100 mA
myLoRa.preamble              = 10;               // default = 8;
```
### Spreading factor values
```bash
//--- SPREADING FACTORS ---//
SF_7      7
SF_8      8
SF_9      9
SF_10     10
SF_11     11
SF_12     12
```
### Bandwidth values
```bash
//------- BANDWIDTH -------//
BW_7_8KHz     7.8   KHz
BW_10_4KHz    10.4  KHz
BW_15_6KHz    15.6  KHz
BW_20_8KHz    20.8  KHz
BW_31_25KHz   31.25 KHz
BW_41_7KHz    41.7  KHz
BW_62_5KHz    62.5  KHz
BW_125KHz     125   KHz
BW_250KHz     250   KHz
BW_500KHz     500   KHz
```
### Coding rate values
```bash
//------ CODING RATE ------//
CR_4_5    4/5
CR_4_6    4/6
CR_4_7    4/7
CR_4_8    4/8
```
### Power values
```bash
//------ POWER GAIN ------//
POWER_11db  11db
POWER_14db  14db
POWER_17db  17db
POWER_20db  20db
```
### Over current protection
The max current must be a multiple of 5 if it is less than 120, and must be a multiple of 10 if it is greater than 120.
Minimum value is 45 mA and maximum is 240 mA. See [this file](https://github.com/SMotlaq/LoRa_third/blob/master/OCP_values.xlsx?raw=true) to know allowed currents.
