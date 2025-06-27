/*
 * funciones_domotica.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_FUNCIONES_DOMOTICA_H_
#define INC_FUNCIONES_DOMOTICA_H_

#include "stm32f1xx_hal.h"
#include "DHT.h"

void init_sensores(ADC_HandleTypeDef*);
void update_DHT (void);
DHT_data get_datosDHT (void);
void update_ldr (void);
uint32_t get_ldr (void);
uint32_t get_umbralLDR (uint8_t);
void set_umbralLDR (uint8_t, uint32_t);
void set_modoLuz (uint8_t);
uint8_t get_modoLuz (void);
void check_luzAuto (void);
uint8_t getStat_rele (void);
void timeoutTestAuto (void);
void timeoutGrabaLuzMan (void);
void start_regHora (void);


#endif /* INC_FUNCIONES_DOMOTICA_H_ */
