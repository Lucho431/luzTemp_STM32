/*
 * lcd_i2c_lfs.c
 *
 *  Created on: Feb 9, 2020
 *      Author: Luciano Salvatore
 */

/** Put this in the src folder **/

#include <lcd_i2c_lfs.h>

I2C_HandleTypeDef* i2c_handler;  // change your handler here accordingly
uint8_t SLAVE_ADDRESS_LCD; //(0x3F)<<1 // change this according to ur setup

void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (i2c_handler, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit (i2c_handler, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int x, int y)
{
    switch (y){
    	case 0:
    		lcd_send_cmd(LCD_DDRAM_ADDRESS + LCD_START_LINE1 + x);
    	break;
    	case 1:
    		lcd_send_cmd(LCD_DDRAM_ADDRESS + LCD_START_LINE2 + x);
    	break;
    	case 2:
    		lcd_send_cmd(LCD_DDRAM_ADDRESS + LCD_START_LINE3 + x);
    	break;
    	case 3:
    		lcd_send_cmd(LCD_DDRAM_ADDRESS + LCD_START_LINE4 + x);
    	break;
    }
}


void lcd_init (I2C_HandleTypeDef* hi2c, uint8_t i2c_address)
{
	i2c_handler = hi2c; //set the i2c handler struct.
	SLAVE_ADDRESS_LCD = (i2c_address << 1); //set the lcd i2c address (left alignment).

	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}


void lcd_CustomChar_create(uint8_t location, uint8_t charmap[]) {
	location <<= 3;

	lcd_send_cmd (0x40 | (location & 0x38) );
	for (int i=0; i<8; i++) {
		lcd_send_data(charmap[i]);
	}
} //fin lcd_CustomChar_create ()
