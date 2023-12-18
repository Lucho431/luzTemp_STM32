/*
 * PCF8574_lfs.h
 *
 *  Created on: 26 jun. 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_PCF8574_LFS_H_
#define INC_PCF8574_LFS_H_

#include "stm32f1xx_hal.h"

#define DEFAULT_ADDR_PCF8574 0x20

void pcf8574_init (I2C_HandleTypeDef*, uint8_t);
uint8_t pcf8574_readPort (void);
void pcf8574_writePort (uint8_t);

#endif /* INC_PCF8574_LFS_H_ */
