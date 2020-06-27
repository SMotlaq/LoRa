#include "stm32f4xx_hal.h"

#define TRANSMIT_TIMEOUT	2000
#define RECEIVE_TIMEOUT		2000

//--------- MODES ---------//
#define SLEEP_MODE				0
#define	STNBY_MODE				1

//------- BANDWIDTH -------//
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

//------ CODING RATE ------//
#define CR_4_5						1
#define CR_4_6						2
#define CR_4_7						3
#define CR_4_8						4

//--- SPREADING FACTORS ---//
#define SF_7							7
#define SF_8							8
#define SF_9							9
#define SF_10							10
#define SF_11  						11
#define SF_12							12

//------ POWER GAIN ------//
#define POWER_11db				0xF6
#define POWER_14db				0xF9
#define POWER_17db				0xFC
#define POWER_20db				0xFF

//------- REGISTERS -------//
#define RegOpMode					0x01
#define RegFrMsb					0x06
#define RegFrMid					0x07
#define RegFrLsb					0x08
#define RegPaConfig				0x09
#define RegOcp						0x0B
#define RegLna						0x0C
#define	RegModemConfig1		0x1D
#define RegModemConfig2		0x1E

typedef struct LoRa_setting{
	
	// Hardware setings:
	GPIO_TypeDef*      CS_port;
	uint16_t			     CS_pin;
	GPIO_TypeDef*      reset_port;
	uint16_t			     reset_pin;
	SPI_HandleTypeDef* hSPIx;
	
	// Module settings:
	int 				frequency;
	uint8_t			spredingFactor;
	uint8_t			bandWidth;
	uint8_t			crcRate;
	float				preamble;
	uint8_t			power;
	uint8_t			overCurrentProtection;
	
} LoRa;

LoRa newLoRa(void);
void LoRa_reset(LoRa* _LoRa);
void LoRa_readReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length);
void LoRa_writeReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length);
void LoRa_gotoMode(LoRa* _LoRa, int mode);
uint8_t LoRa_read(LoRa* _LoRa, uint8_t address);
void LoRa_write(LoRa* _LoRa, uint8_t address, uint8_t value);
uint8_t LoRa_isvalid(LoRa* _LoRa);

void LoRa_setFrequency(LoRa* _LoRa, int freq);
void LoRa_setSpreadingFactor(LoRa* _LoRa, int SP);
void LoRa_setPower(LoRa* _LoRa, uint8_t power);
void LoRa_setOCP(LoRa* _LoRa, uint8_t current);
void LoRa_init(LoRa* _LoRa);
