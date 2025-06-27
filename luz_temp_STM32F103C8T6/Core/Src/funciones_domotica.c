/*
 * funciones_domotica.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Luciano Salvatore
 */

#include "funciones_domotica.h"
#include "menuPantallas.h"
#include "IOport_lfs.h"
#include "24lc256_lfs.h"
#include "hora_tablero.h"

//#define TESTING
#define MAX_SAVELUZMAN 5000;

//variables
static uint32_t lecturaLDR;
ADC_HandleTypeDef* hadc;
static uint32_t umbralMaxLDR = 1200; //valor estimado a priori
static uint32_t umbralMinLDR = 275; //valor estimado a priori
uint8_t modoAuto = 1; //0 -> manual; 1 -> automatico.
static DHT_sensor sensorDHT = {GPIOB, GPIO_PIN_13, DHT11, GPIO_NOPULL};
uint8_t estadoRele;
DHT_data datosDHT;
uint8_t flag_LDR = 0;
uint8_t acum_umbral = 0;
uint16_t timeout_saveLuzMan = MAX_SAVELUZMAN;
//variables de prueba
uint32_t timeout_auto = 0; // cuenta hasta 90000 ms (15 minutos)
uint32_t timeout_regHora = 0; //cuenta hasta 180000 ms (30 minutos)
uint8_t flag_regHora = 0; //indica si se activo el registro de hora de apagado
RTC_TimeTypeDef auxHoraOff;
RTC_DateTypeDef auxFechaOff;
//variables externas
extern I2C_HandleTypeDef hi2c1;
extern uint8_t max_periodoTest; //49; //24;


void init_sensores (ADC_HandleTypeDef* handler_adc){
	hadc = handler_adc;

	HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_MODO_AUTO, I2C_MEMADD_SIZE_16BIT, &modoAuto, 1, 100);
	setOutput(OUT_MODO, !modoAuto); //LOGICA NEGATIVA

	HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_ESTADO_LUZ, I2C_MEMADD_SIZE_16BIT, &estadoRele, 1, 100);
	HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, estadoRele); //LOGICA POSITIVA
} //fin init_sensores()


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
	HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_MODO_AUTO, I2C_MEMADD_SIZE_16BIT, &modoAuto, 1, 100);
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
				HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, estadoRele); //LOGICA POSITIVA
#ifdef TESTING
				if (estadoRele != 0){
					max_periodoTest = 24;
				}else{
					max_periodoTest = 49;
				}
#endif
				if (!timeout_saveLuzMan){ //graba el estado en manual solo si pasaron 5 segundos del último cambio
					HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_ESTADO_LUZ, I2C_MEMADD_SIZE_16BIT, &estadoRele, 1, 100);
					timeout_saveLuzMan = MAX_SAVELUZMAN;
				} //fin if !timeout_saveLuzMan
				refresh_infoModo();
			} //fin if IN_LUZ
		break;
		case 1:
			if (getStatBoton(IN_MODO) == FALL){
				set_modoLuz(0);
				refresh_infoModo();
				break;
			} //fin if IN_MODO

#ifdef TESTING
			if (timeout_auto > 90000){ //pasaron 15 minutos
				estadoRele = !estadoRele;
				timeout_auto = 0;
				HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, !estadoRele);
				if (estadoRele != 0){
					max_periodoTest = 24;
				}else{
					max_periodoTest = 49;
				}
				setOutput(OUT_LUZ, estadoRele); //LOGICA NEGATIVA
				refresh_infoModo();
				if (flag_regHora != 0){
					update_horaFecha();
					auxHoraOff = get_hora();
					auxFechaOff = get_fecha();
					HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_HORA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&auxHoraOff, 3, 100);
					HAL_Delay(10);
					HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_FECHA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&auxFechaOff, 4, 100);
				} //fin if flag_regHora
			}
#else
			switch (estadoRele){ //LOGICA POSITIVA
				case 0: //luz apagada
					if (!flag_LDR) break;

					if (lecturaLDR < umbralMinLDR){ //si es de noche
						acum_umbral++;
					}else{
						acum_umbral = 0;
					} //fin if lecturaLDR...

					if (acum_umbral > 2){ //3 segundos por debajo del umbral
						estadoRele = 1;
						HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, estadoRele); //LOGICA POSITIVA
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_ESTADO_LUZ, I2C_MEMADD_SIZE_16BIT, &estadoRele, 1, 100);
						refresh_infoModo();
						acum_umbral = 0;
						break;
					} //fin if acum_umbral
				break;
				case 1: //luz prendida
					if (!flag_LDR) break;

					if (lecturaLDR > umbralMaxLDR){ //si es de día
						acum_umbral++;
					}else{
						acum_umbral = 0;
					} //fin if lecturaLDR...

					if (acum_umbral > 2){ //3 segundos por encima del umbral
						estadoRele = 0;
						HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, estadoRele); //LOGICA POSITIVA
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_ESTADO_LUZ, I2C_MEMADD_SIZE_16BIT, &estadoRele, 1, 100);
						refresh_infoModo();
						acum_umbral = 0;
						break;
					} //fin if acum_umbral
				break;
				default:
				break;
			} //fin switch estadoRele
#endif
			flag_LDR = 0;
		break;
		default:
		break;
	} //fin switch modoAuto
} //fin check_luzAuto()


uint8_t getStat_rele (void){
	return estadoRele;
} //fin get_rele()


void timeoutTestAuto (void){
	timeout_auto++;
} //fin timeoutTestAuto()


void timeoutGrabaLuzMan (void){
	if (timeout_saveLuzMan != 0) timeout_saveLuzMan--;
} //fin timeoutGrabaLuzMan()

void start_regHora (void){
	flag_regHora = 1;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
} //fin start_regHora()
