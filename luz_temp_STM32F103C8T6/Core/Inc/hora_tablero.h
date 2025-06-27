/*
 * hora_tablero.h
 *
 *  Created on: 30 jun. 2022
 *      Author: Luciano Salvatore
 */

#ifndef INC_HORA_TABLERO_H_
#define INC_HORA_TABLERO_H_

#include "rtc.h"

void update_horaFecha (void);
void set_hora (void);
void set_fecha (void);
RTC_TimeTypeDef get_hora (void);
RTC_DateTypeDef get_fecha (void);

#endif /* INC_HORA_TABLERO_H_ */
