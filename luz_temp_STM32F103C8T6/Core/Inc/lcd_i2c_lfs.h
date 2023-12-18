/*
 * lcd_i2c_lfs.h
 *
 *  Created on: Feb 9, 2020
 *      Author: Luciano Salvatore
 */

#ifndef LCD_I2C_LFS_H_
#define LCD_I2C_LFS_H_


#include "stm32f1xx_hal.h"

#define lcd_send_customChar(X) ( lcd_send_data(X) )
#define lcd_blinkCursOff ( lcd_send_cmd (0x0C) )
#define lcd_blinkCursOn  ( lcd_send_cmd (0xD) )

#define LCD_DDRAM_ADDRESS	  0x80 // 0b10000000
#define LCD_START_LINE1 	  0x00
#define LCD_START_LINE2 	  0x40
#define LCD_START_LINE3 	  0x14
#define LCD_START_LINE4 	  0x54

void lcd_init (I2C_HandleTypeDef*,uint8_t);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_CustomChar_create(uint8_t, uint8_t[]);

#endif /* LCD_I2C_LFS_H_ */
