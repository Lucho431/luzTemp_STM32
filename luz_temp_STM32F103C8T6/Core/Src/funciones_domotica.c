/*
 * funciones_domotica.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Luciano Salvatore
 */

#include "funciones_domotica.h"
#include "menuPantallas.h"
#include "IOport_lfs.h"

//variables
static uint32_t lecturaLDR;
ADC_HandleTypeDef* hadc;
static uint32_t umbralMaxLDR = 1200; //valor estimado a priori
static uint32_t umbralMinLDR = 275; //valor estimado a priori
uint8_t modoAuto = 0; //0 -> manual; 1 -> automatico.
static DHT_sensor sensorDHT = {GPIOB, GPIO_PIN_13, DHT11, GPIO_NOPULL};
uint8_t estadoRele;
DHT_data datosDHT;
uint8_t flag_LDR = 0;
uint8_t acum_umbral = 0;


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
	flag_LDR = 1;
} //fin if update_ldr()


uint32_t get_ldr (void){
	return lecturaLDR;
} //fin get_ldr()


uint32_t get_umbralLDR (uint8_t u){
	if (u != 0) return umbralMaxLDR;
	return umbralMinLDR;
} //fin get_umbralLDR


void set_umbralLDR (uint8_t u, uint32_t val){
	if (u != 0){
		umbralMaxLDR = val;
		return;
	}

	umbralMinLDR = val;
} //fin set_umbralLDR


void set_modoLuz (uint8_t val){
	modoAuto = val;
	setOutput(OUT_MODO, !modoAuto); //LOGICA NEGATIVA
} //fin set_modoLuz()


uint8_t get_modoLuz (void){
	return modoAuto;
} //fin get_modoLuz()

void check_luzAuto (void){
	switch (modoAuto) {
		case 0:
			if (getStatBoton(IN_MODO) == FALL){
				set_modoLuz(1);
				refresh_infoModo();
				break;
			} //fin if IN_MODO

			if (getStatBoton(IN_LUZ) == FALL){
				estadoRele = !estadoRele;
				HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, !estadoRele); //LOGICA NEGATIVA
				setOutput(OUT_LUZ, estadoRele); //LOGICA NEGATIVA

				refresh_infoModo();
			} //fin if IN_LUZ
		break;
		case 1:
			if (getStatBoton(IN_MODO) == FALL){
				set_modoLuz(0);
				refresh_infoModo();
				break;
			} //fin if IN_MODO

			switch (estadoRele){ //LOGICA NEGATIVA
				case 1:
					if (!flag_LDR) break;

					if (lecturaLDR < umbralMinLDR){
						acum_umbral++;
					}else{
						acum_umbral = 0;
					} //fin if lecturaLDR...

					if (acum_umbral > 2){ //3 segundos por debajo del umbral
						estadoRele = 0;
						HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, !estadoRele);
						setOutput(OUT_LUZ, estadoRele); //LOGICA NEGATIVA
						refresh_infoModo();
						acum_umbral = 0;
						break;
					} //fin if acum_umbral
				break;
				case 0:
					if (!flag_LDR) break;

					if (lecturaLDR > umbralMaxLDR){
						acum_umbral++;
					}else{
						acum_umbral = 0;
					} //fin if lecturaLDR...

					if (acum_umbral > 2){ //3 segundos por encima del umbral
						estadoRele = 1;
						HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, !estadoRele);
						setOutput(OUT_LUZ, estadoRele); //LOGICA NEGATIVA
						refresh_infoModo();
						acum_umbral = 0;
						break;
					} //fin if acum_umbral
				break;
				default:
				break;
			} //fin switch estadoRele

			flag_LDR = 0;
		break;
		default:
		break;
	} //fin switch modoAuto
} //fin check_luzAuto()


uint8_t getStat_rele (void){
	return estadoRele;
} //fin get_rele ()
