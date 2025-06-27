/*
 * 24lc256_lfs.h
 *
 *  Created on: 03 ago 2023
 *      Author: Luciano Salvatore
 */

#ifndef MAIN_24LC256_LFS_H_
#define MAIN_24LC256_LFS_H_

//#include "stm32f1xx_hal.h"
#include "main.h"


//definiciones de offset de memoria
#define OFFSET_FIRMA 0
#define OFFSET_RESETS 3
#define OFFSET_ESTADO_LUZ 4
#define OFFSET_MODO_AUTO 5
#define OFFSET_MAX_LDR 6
#define OFFSET_MIN_LDR 10
#define OFFSET_HORA_OFF 14
#define OFFSET_FECHA_OFF 17
#define OFFSET_HORA_ON 21
#define OFFSET_FECHA_ON 24
#define LAST_ADDRESS_256 32768
#define LAST_ADDRESS_512 65535 //131071


void mem_24LC256_init (I2C_HandleTypeDef*, uint8_t);
void mem_24LC256_readPlan (uint8_t, uint8_t[]);
void mem_24LC256_writePlan (uint8_t, uint8_t[]);
void mem_24LC256_readIntervalo (uint8_t, uint8_t, uint8_t[]);
void mem_24LC256_writeIntervalo (uint8_t, uint8_t, uint8_t[]);

#endif /* MAIN_24LC256_LFS_H_ */
