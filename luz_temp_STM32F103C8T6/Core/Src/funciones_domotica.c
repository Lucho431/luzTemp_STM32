/*
 * funciones_domotica.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Luciano Salvatore
 */

#include "funciones_domotica.h"


//variables
static uint32_t lecturaLDR;
ADC_HandleTypeDef* hadc;
static uint32_t umbralMaxLDR;
static uint32_t umbralMinLDR;
uint8_t modoAuto = 0;
static DHT_sensor sensorDHT = {GPIOB, GPIO_PIN_13, DHT11, GPIO_NOPULL};
DHT_data datosDHT;

void init_sensores (ADC_HandleTypeDef* handler_adc){
	hadc = handler_adc;

}


void update_DHT (void){
	datosDHT = DHT_getData(&sensorDHT);
} //fin update_temp()


DHT_data get_datosDHT (void){
	return datosDHT;
} //fin get_datosDHT()


void update_ldr (void){
	lecturaLDR = HAL_ADC_GetValue(hadc);
} //fin if update_ldr()


uint32_t get_ldr (void){
	return lecturaLDR;
} //fin get_ldr()


void set_modoLuz (uint8_t val){
	modoAuto = val;
} //fin set_modoLuz()


uint8_t get_modoLuz (void){
	return modoAuto;
} //fin get_modoLuz()
