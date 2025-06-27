/*
 * menuPantallas.h
 *
 *  Created on: Nov 30, 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_MENUPANTALLAS_H_
#define INC_MENUPANTALLAS_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"

typedef enum{
	MENU_INFO,
	MENU_SELECCION,
	MENU_MODO_LUZ,
	MENU_LDR_PRENDE,
	MENU_LDR_APAGA,
	MENU_SET_HORA,
	MENU_DEBUG,
	SIZE_MENU_NOMBRE,
} T_MENU_NOMBRE;

typedef enum{
	PANT_DEBUG,
	PANT_REG_OFF,
	PANT_TIEMPO_VIVO,

	PANT_BORRAR_RESETS,
	PANT_RESET_BORRADOS,
} T_PANT_DEBUG;

typedef enum{
	PANT_HORA_FECHA,
	PANT_CONFIR_H_F,
	PANT_GRABADO_H_F,
	PANT_ERROR_H_F,
}T_PANT_HORA_FECHA;

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
uint8_t calculaDiaSemana (int, int, int);

#endif /* INC_MENUPANTALLAS_H_ */
