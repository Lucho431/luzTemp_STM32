/*
 * menuPantallas.h
 *
 *  Created on: Nov 30, 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_MENUPANTALLAS_H_
#define INC_MENUPANTALLAS_H_

#include "stm32f1xx_hal.h"

typedef enum{
	MENU_INFO,
	MENU_SELECCION,
	MENU_MODO_LUZ,
	MENU_LDR_PRENDE,
	MENU_LDR_APAGA,
	SIZE_MENU_NOMBRE,
} T_MENU_NOMBRE;

typedef struct t_menu {
	T_MENU_NOMBRE nombre;
	struct t_menu* menuAnterior;
	void (*inicia_menu) (void);
	void (*accion) (void);
} T_MENU;

typedef enum{
	INICIANDO_MENU,
	PRESENTA_INFO,
} T_STATUS_MENU_INFO;


//funciones publicas
void start_menu (uint8_t);
void check_menu (void);
void check_pulsadores (void);
void check_duracionPulsadores (void);
void timeoutMenu (void);
void refresh_infoDHT (void);
void refresh_infoModo (void);

#endif /* INC_MENUPANTALLAS_H_ */
