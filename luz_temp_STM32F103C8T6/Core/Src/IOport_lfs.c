/*
 * IOport_lfs.c
 *
 *  Created on: Dec 5, 2023
 *      Author: Luciano Salvatore
 */

#include "IOport_lfs.h"
#include "PCF8574_lfs.h"

//variables
uint8_t flag_sinBotones = 1;

static uint8_t read_input, last_input, fall_input, rise_input;
static uint8_t write_output = 0xFF;


void init_botonera (I2C_HandleTypeDef* i2c_handler, uint8_t i2c_address){

	pcf8574_init(i2c_handler, i2c_address);

	//lectura inicial
	flag_sinBotones = HAL_I2C_Master_Receive(i2c_handler, i2c_address << 1, &read_input, 1, 100);
	last_input = read_input;

	pcf8574_writePort(0xFF); //weak pullup para todos los pines
} //fin init_botonera()


void lecturaTeclas(void){
	if (flag_sinBotones != HAL_OK) return;
	read_input = pcf8574_readPort();
} //fin lecturaTeclas()

void update_teclas (void){

	fall_input = last_input & ~read_input;
	rise_input = ~last_input & read_input;

	last_input = read_input;

} //fin update_teclas()


T_INPUT getStatBoton (T_POS_INPUT b){

	if (flag_sinBotones != 0) return HIGH_L;

	if ( (fall_input & (1 << b)) != 0)
		return FALL;

	if ( (rise_input & (1 << b)) != 0)
		return RISE;

	if ( (read_input & (1 << b)) != 0)
		return HIGH_L;

	return LOW_L;
} //fin getStatBoton()


uint8_t detectaAlgunBoton (void){
	return 0;
} //fin detectaAgunBoton()


void setOutput (T_POS_OUTPUT s, uint8_t val){

	if (val != 0){
		write_output |= (uint8_t)(1 << (s + 6));
		return;
	}else{
		write_output &= ~( (uint8_t)(1 << (s + 6)) );
	} //fin if val

} //fin setOutput()

void update_outputs (void){

	pcf8574_writePort(write_output);

}
