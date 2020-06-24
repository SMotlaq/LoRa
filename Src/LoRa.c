#include "LoRa.h"

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_reset

		description : reset module
		
		arguments   : 
			LoRa* LoRa --> LoRa object handler
		
		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_reset(LoRa* _LoRa){
	HAL_GPIO_WritePin(_LoRa->reset_port, _LoRa->reset_pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(_LoRa->reset_port, _LoRa->reset_pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_gotoMode

		description : set LoRa Op mode

		arguments   : 
			LoRa* LoRa    --> LoRa object handler
			mode	        --> select from defined modes

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_gotoMode(LoRa* _LoRa, int mode){
	uint8_t    read[1];
	uint8_t address[1];
	uint8_t    data[1];
	address[0] = RegOpMode & 0x7F;
	LoRa_readReg(_LoRa, address, 1, read, 1);
	
	if(mode == SLEEP_MODE)
		data[0] = read[0] & 0xF8;
	else
		data[0] = (read[0] & 0xF8) | 0x01;
	
	address[0] = RegOpMode | 0x80;
	LoRa_writeReg(_LoRa, address, 1, data, 1);
}


/* ----------------------------------------------------------------------------- *\
		name        : LoRa_readReg

		description : read a register(s) by an address and a length,
									then store value(s) at outpur array.
		arguments   : 
			LoRa* LoRa        --> LoRa object handler
			uint8_t* address  -->	pointer to the beginning of address array
			uint16_t r_length -->	detemines number of addresse bytes that
														you want to send
			uint8_t* output		--> pointer to the beginning of output array
			uint16_t w_length	--> detemines number of bytes that you want to read

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_readReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length){
	HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(_LoRa->hSPIx, address, r_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY)
		;
	HAL_SPI_Receive(_LoRa->hSPIx, output, w_length, RECEIVE_TIMEOUT);
	while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY)
		;
	HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_writeReg

		description : write a value(s) in a register(s) by an address,

		arguments   : 
			LoRa* LoRa        --> LoRa object handler
			uint8_t* address  -->	pointer to the beginning of address array
			uint16_t r_length -->	detemines number of addresse bytes that
														you want to send
			uint8_t* output		--> pointer to the beginning of values array
			uint16_t w_length	--> detemines number of bytes that you want to send

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_writeReg(LoRa* _LoRa, uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length){
	HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(_LoRa->hSPIx, address, r_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY)
		;
	HAL_SPI_Transmit(_LoRa->hSPIx, values, w_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY)
		;
	HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
}
