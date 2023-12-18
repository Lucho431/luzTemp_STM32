/*
 * menuPantallas.c
 *
 *  Created on: Nov 30, 2023
 *      Author: Luciano Salvatore
 */

#include <menuPantallas.h>
#include "funciones_domotica.h"
#include "lcd_i2c_lfs.h"
#include "IOport_lfs.h"
#include "DHT.h"
#include <stdio.h>

uint8_t arriba[8] = {
		0b00000,
		0b00100,
		0b01110,
		0b10101,
		0b00100,
		0b00100,
		0b00000,
		0b00000,
};

uint8_t abajo [8] = {
		0b00000,
		0b00000,
		0b00100,
		0b00100,
		0b10101,
		0b01110,
		0b00100,
		0b00000,
};


//variables menu
T_MENU* menuActual;
T_MENU* menuAux;
static char texto[50];
uint16_t timeOut_pantalla = 0;
//variables menu info
DHT_data sensorDHT;
uint8_t flag_infoDHT = 0;
//variables menu seleccion
uint8_t cursor = 0;
//variables menu modo luz
uint8_t modoLuz;


void acc_Info (void);
void acc_Seleccion (void);
void acc_ModoLuz (void);
void acc_LdrPrende (void);
void acc_LdrApaga (void);

void init_Info (void);
void init_Seleccion (void);
void init_ModoLuz (void);
void init_LdrPrende (void);
void init_LdrApaga (void);

T_MENU menu[SIZE_MENU_NOMBRE] = {
		{MENU_INFO, NULL, init_Info, acc_Info}, //MENU_INFO,
		{MENU_SELECCION, NULL, init_Seleccion, acc_Seleccion}, //MENU_SELECCION,
		{MENU_MODO_LUZ, NULL, init_ModoLuz, acc_ModoLuz}, //MENU_MODO_LUZ,
		{MENU_LDR_PRENDE, NULL, init_LdrPrende, acc_LdrPrende}, //MENU_LDR_PRENDE,
		{MENU_LDR_APAGA, NULL, init_LdrApaga, acc_LdrApaga}, //MENU_LDR_APAGA,
};

/////////////////////////////////////////
//         FUNCIONES PUBLICAS          //
/////////////////////////////////////////
void start_menu (uint8_t service){
	lcd_CustomChar_create(0, arriba);
	lcd_CustomChar_create(1, abajo);
	menuActual = &menu[MENU_INFO];
	menuActual->inicia_menu();
} //fin start_menu()


void check_menu (void){
	menuActual->accion();
} //fin check_menu()


void timeoutMenu (void){
	timeOut_pantalla++;
} //fin timeoutMenu()


/////////////////////////////////////////
//          INICIALIZADORES            //
/////////////////////////////////////////

void init_Info (void){

	sensorDHT = get_datosDHT();

	lcd_clear();
	lcd_put_cur(0, 0);
	sprintf(texto, "Temp: %02dºC", sensorDHT.temp);
	lcd_send_string(texto);
	lcd_put_cur(0, 1);
	sprintf(texto, "Humedad: %02d%%", sensorDHT.hum);
	lcd_send_string(texto);
} //fin init_Info()


void init_Seleccion (void){
	lcd_clear();
	lcd_put_cur(6, 0);
	lcd_send_string("MENU");
	lcd_put_cur(0, 1);
	lcd_send_data(0x7F); //<-
	switch (cursor){
		case 0:
			lcd_put_cur(2, 1);
			lcd_send_string("MODO DE LUZ");
		break;
		case 1:
			lcd_put_cur(3, 1);
			lcd_send_string("UMBRAL DIA");
		break;
		case 2:
			lcd_put_cur(2, 1);
			lcd_send_string("UMBRAL NOCHE");
		break;
		default:
		break;
	} //fin switch cursor
	lcd_put_cur(15, 1);
	lcd_send_data(0x7E); //->
} //fin init_Seleccion()


void init_ModoLuz (void){
	lcd_clear();
	lcd_put_cur(2, 0);
	lcd_send_string("MODO DE LUZ");
	lcd_put_cur(2, 1);
	lcd_send_data(0x7F); //<-
	lcd_put_cur(7, 1);
	switch (modoLuz) {
		case 0:
			lcd_send_string("OFF");
		break;
		case 1:
			lcd_send_string("ON");
		break;
		default:
		break;
	} //fin switch modoLuz
	lcd_put_cur(13, 1);
	lcd_send_data(0x7E); //->
} //fin init_ModoLuz()


void init_LdrPrende (void){

} //fin init_LdrPrende()


void init_LdrApaga (void){

} //fin init_LdrApaga()

/////////////////////////////////////////
//            ACCIONES                 //
/////////////////////////////////////////

void acc_Info (void){

	if (flag_infoDHT != 0){
		sensorDHT = get_datosDHT();

		lcd_put_cur(6, 0);
		sprintf(texto, "%02dºC ", sensorDHT.temp);
		lcd_send_string(texto);
		lcd_put_cur(9, 1);
		sprintf(texto, "%02d%% ", sensorDHT.hum);
		lcd_send_string(texto);

		flag_infoDHT = 0;
	} //fin if flag_infoDHT

	if (getStatBoton(IN_OK) == FALL){
		menuActual = &menu[MENU_SELECCION];
		menuActual->inicia_menu();
	} //fin if IN_OK
} //fin acc_Info()


void acc_Seleccion (void){

	if (getStatBoton(IN_BACK) == FALL){
		menuActual = &menu[MENU_INFO];
		menuActual->inicia_menu();
	} //fin if IN_BACK

	if (getStatBoton(IN_LEFT) == FALL){
		cursor--;
		if (cursor > 2) cursor = 2;

		lcd_put_cur(1, 1);
		switch (cursor){
			case 0:
				lcd_send_string(" MODO DE LUZ  ");
			break;
			case 1:
				lcd_send_string("  UMBRAL DIA  ");
			break;
			case 2:
				lcd_send_string(" UMBRAL NOCHE ");
			break;
			default:
			break;
		} //fin switch cursor
	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){
		cursor++;
		if (cursor > 2) cursor = 0;

		lcd_put_cur(1, 1);
		switch (cursor){
			case 0:
				lcd_send_string(" MODO DE LUZ  ");
			break;
			case 1:
				lcd_send_string("  UMBRAL DIA  ");
			break;
			case 2:
				lcd_send_string(" UMBRAL NOCHE ");
			break;
			default:
			break;
		} //fin switch cursor
	} //fin if IN_RIGHT

	if (getStatBoton(IN_OK) == FALL){
		switch (cursor){
			case 0:
				menuActual = &menu[MENU_MODO_LUZ];
			break;
			case 1:
				menuActual = &menu[MENU_LDR_APAGA];
			break;
			case 2:
				menuActual = &menu[MENU_LDR_PRENDE];
			break;
			default:
			break;
		} //fin switch cursor
		menuActual->inicia_menu();
	} //fin if IN_OK

} //fin acc_Seleccion()


void acc_ModoLuz (void){
	if (getStatBoton(IN_BACK) == FALL){
		menuActual = &menu[MENU_INFO];
		menuActual->inicia_menu();
	} //fin if IN_BACK

	if (getStatBoton(IN_LEFT) == FALL){
		if (modoLuz != 0){
			modoLuz = 1;
		}else{
			modoLuz = 0;
		}
	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){
		if (modoLuz != 0){
			modoLuz = 1;
		}else{
			modoLuz = 0;
		}
	} //fin if IN_RIGHT

	if (getStatBoton(IN_OK) == FALL){
		if (modoLuz != 0){
			setOutput(OUT_MODO, 0); //logica negativa
		}else{
			setOutput(OUT_MODO, 1); //logica negativa
		}
		set_modoLuz(modoLuz);
	} //fin if IN_OK
} //fin acc_ModoLuz()


void acc_LdrPrende (void){
	if (getStatBoton(IN_BACK) == FALL){
		menuActual = &menu[MENU_INFO];
		menuActual->inicia_menu();
	} //fin if IN_BACK

	if (getStatBoton(IN_LEFT) == FALL){

	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){

	} //fin if IN_RIGHT

	if (getStatBoton(IN_OK) == FALL){

	} //fin if IN_OK
} //fin acc_LdrPrende()


void acc_LdrApaga (void){
	if (getStatBoton(IN_BACK) == FALL){
		menuActual = &menu[MENU_INFO];
		menuActual->inicia_menu();
	} //fin if IN_BACK

	if (getStatBoton(IN_LEFT) == FALL){

	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){

	} //fin if IN_RIGHT

	if (getStatBoton(IN_OK) == FALL){

	} //fin if IN_OK
} //fin acc_LdrApaga()


void refresh_infoDHT (void){
	flag_infoDHT = 1;
} //fin refresh_infoDHT()
