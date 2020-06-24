#include "stm32f4xx_hal.h"

#define TRANSMIT_TIMEOUT	2000
#define RECEIVE_TIMEOUT		2000

#define SLEEP_MODE				0
#define	STNBY_MODE				1

#define BW_7_8KHz					0
#define BW_10_4KHz				1
#define BW_15_6KHz				2
#define BW_20_8KHz				3
#define BW_31_25KHz				4
#define BW_41_7KHz				5
#define BW_62_5KHz				6
#define BW_125KHz					7
#define BW_250KHz					8
#define BW_500KHz					9

#define CR_4_5						1
#define CR_4_6						2
#define CR_4_7						3
#define CR_4_8						4

#define RegOpMode					0x01
#define	RegModemConfig1		0x1D
#define RegModemConfig2		0x1E
#define RegFrMsb					0x06
#define RegFrMid					0x07
#define RegFrLsb					0x08

#define POWER_17db				0
#define POWER_20db				1

typedef struct LoRa_setting{
	GPIO_TypeDef*      CS_port;
	uint16_t			     CS_pin;
	GPIO_TypeDef*      reset_port;
	uint16_t			     reset_pin;
	SPI_HandleTypeDef* hSPIx;
	
	int 					     frequency;
	uint8_t						 spredingFactor;
	uint8_t						 bandWidth;
	uint8_t						 crcRate;
	float							 preamble;
	
} LoRa;

void LoRa_reset(LoRa* _LoRa);
void LoRa_readReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length);
void LoRa_writeReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length);
void LoRa_gotoMode(LoRa* _LoRa, int mode);

void LoRa_setFrequency(LoRa* _LoRa, int freq);
void LoRa_setSpreadingFactor(LoRa* _LoRa, int SP);

void LoRa_init(LoRa* _LoRa);
