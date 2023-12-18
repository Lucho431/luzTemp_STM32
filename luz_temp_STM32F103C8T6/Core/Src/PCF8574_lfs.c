/*
 * PCF8574_lfs.c
 *
 *  Created on: 26 jun. 2023
 *      Author: Luciano Salvatore
 */

#include "PCF8574_lfs.h"

static I2C_HandleTypeDef* i2c_handler;  // change your handler here accordingly
uint8_t addressPCF8574; //(0x20)<<1 // change this according to ur setup

void pcf8574_init (I2C_HandleTypeDef* hi2c, uint8_t i2c_address){
	i2c_handler = hi2c; //set the i2c handler struct.
	addressPCF8574 = (i2c_address << 1); //set the lcd i2c address (left alignment).
} //fin pcf8574_init()

uint8_t pcf8574_readPort (void){
	uint8_t reg;
	HAL_I2C_Master_Receive(i2c_handler, addressPCF8574, &reg, 1, 100);
	return reg;
} //fin pcf8574_readInputs()

void pcf8574_writePort (uint8_t reg){
	HAL_I2C_Master_Transmit(i2c_handler, addressPCF8574, &reg, 1, 100);
} //fin pcf8574_writeOutputs
