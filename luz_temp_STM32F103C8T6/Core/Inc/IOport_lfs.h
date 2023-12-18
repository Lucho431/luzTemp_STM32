/*
 * IOport_lfs.h
 *
 *  Created on: Dec 5, 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_IOPORT_LFS_H_
#define INC_IOPORT_LFS_H_

#include "stm32f1xx_hal.h"

typedef enum{
    LOW_L,
    HIGH_L,
    FALL,
    RISE,
}T_INPUT;

typedef enum{
	IN_LEFT,
	IN_RIGHT,
	IN_BACK,
	IN_OK,
	IN_LUZ,
	IN_MODO,
	SIZEOF_POS_INPUT,
}T_POS_INPUT;

typedef enum{
	OUT_LUZ,
	OUT_MODO,
	SIZEOF_POS_OUTPUT,
}T_POS_OUTPUT;

void init_botonera (I2C_HandleTypeDef*, uint8_t);
void lecturaTeclas(void);
void update_teclas (void);
T_INPUT getStatBoton (T_POS_INPUT);
uint8_t detectaAlgunBoton (void);
void setOutput (T_POS_OUTPUT, uint8_t);
void update_outputs (void);

#endif /* INC_IOPORT_LFS_H_ */
