/*
 * hora_tablero.c
 *
 *  Created on: 30 jun. 2022
 *      Author: Luciano Salvatore
 */

#include "hora_tablero.h"

RTC_TimeTypeDef hora;
RTC_DateTypeDef aux_fecha;

void update_horaFecha (void){
	 //segun el proyecto "MAQUINA_MD":
	  HAL_RTC_GetTime(&hrtc, &hora, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &aux_fecha, RTC_FORMAT_BIN);
}

void set_hora (void){
	__NOP();
}

void set_fecha (void){
	__NOP();
}

RTC_TimeTypeDef get_hora (void){
	return hora;
}

RTC_DateTypeDef get_fecha (void){
	return aux_fecha;
}
