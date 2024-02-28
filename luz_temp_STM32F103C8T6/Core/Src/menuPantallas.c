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

uint8_t grados [8] = {
		0b00110,
		0b01001,
		0b01001,
		0b00110,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
};


//variables menu
T_MENU* menuActual;
T_MENU* menuAux;
static char texto[50];
uint16_t timeOut_pantalla = 0;
//variables menu info
DHT_data sensorDHT;
int8_t temperatura;
int8_t humedad;
uint8_t flag_infoDHT = 0;
//variables menu seleccion
uint8_t cursor = 0;
//variables menu modo luz
uint8_t modoLuz;
uint8_t aux_modoLuz;
//variables menu umbral
uint8_t pantallaUmbral = 0;
uint32_t valorUmbral;
//variables de botones
uint8_t holdBoton = 0;
uint8_t flag_holdBoton = 0;
uint8_t repitePulso = 0;

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
	lcd_CustomChar_create(2, grados);
	menuActual = &menu[MENU_INFO];
	menuActual->inicia_menu();
} //fin start_menu()


void check_menu (void){
	menuActual->accion();
} //fin check_menu()


void timeoutMenu (void){
	timeOut_pantalla++;
} //fin timeoutMenu()


void check_duracionPulsadores (void){
	holdBoton++;
	repitePulso++;
} //fin duracionPulsadores()

/////////////////////////////////////////
//          INICIALIZADORES            //
/////////////////////////////////////////

void init_Info (void){

	sensorDHT = get_datosDHT();
	temperatura = sensorDHT.temp;
	humedad = sensorDHT.hum;

	lcd_clear();
	lcd_put_cur(0, 0);
	sprintf(texto, "Temp: %02d C", temperatura);
	lcd_send_string(texto);
	lcd_put_cur(8, 0);
	lcd_send_customChar(2); //grados
	lcd_put_cur(0, 1);
	sprintf(texto, "Humedad: %02d%%", humedad);
	lcd_send_string(texto);
	lcd_put_cur(0, 2);
	lcd_send_string("Luz: ");
	lcd_send_string( (getStat_rele() != 0) ? "APAGADA" : "PRENDIDA");
	lcd_put_cur(0, 3);
	lcd_send_string((get_modoLuz() != 0) ? "AUTOMATICO" : "MANUAL");
} //fin init_Info()


void init_Seleccion (void){
	lcd_clear();
	lcd_put_cur(7, 0);
	lcd_send_string("MENU");
	lcd_put_cur(2, 2);
	lcd_send_data(0x7F); //<-
	switch (cursor){
		case 0:
			lcd_put_cur(4, 2);
			lcd_send_string("MODO DE LUZ");
		break;
		case 1:
			lcd_put_cur(5, 2);
			lcd_send_string("UMBRAL DIA");
		break;
		case 2:
			lcd_put_cur(4, 2);
			lcd_send_string("UMBRAL NOCHE");
		break;
		default:
		break;
	} //fin switch cursor
	lcd_put_cur(17, 2);
	lcd_send_data(0x7E); //->
} //fin init_Seleccion()


void init_ModoLuz (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("MODO LUZ AUTOMATICA");
	lcd_put_cur(4, 2);
	lcd_send_data(0x7F); //<-
	lcd_put_cur(9, 2);
	aux_modoLuz = modoLuz;
	switch (aux_modoLuz) {
		case 0:
			lcd_send_string("OFF");
		break;
		case 1:
			lcd_send_string("ON");
		break;
		default:
		break;
	} //fin switch modoLuz
	lcd_put_cur(15, 2);
	lcd_send_data(0x7E); //->
} //fin init_ModoLuz()


void init_LdrPrende (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("    UMBRAL NOCHE    ");
	valorUmbral = get_umbralLDR(0);
	lcd_put_cur(0, 1);
	sprintf(texto, "VALOR GRABADO: %04lu", valorUmbral);
	lcd_send_string(texto);
	lcd_put_cur(0, 2);
	sprintf(texto, "VALOR ACTUAL: %04lu", get_ldr());
	lcd_send_string(texto);
	lcd_put_cur(0, 3);
	lcd_send_string("VALOR NUEVO: ");
	sprintf(texto, "%c%04lu%c", 0x7F, valorUmbral, 0x7E);
	lcd_send_string(texto);
	pantallaUmbral = 0;
	timeOut_pantalla = 0;
} //fin init_LdrPrende()


void init_LdrApaga (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("     UMBRAL DIA     ");
	valorUmbral = get_umbralLDR(1);
	lcd_put_cur(0, 1);
	sprintf(texto, "VALOR GRABADO: %04lu", valorUmbral);
	lcd_send_string(texto);
	lcd_put_cur(0, 2);
	sprintf(texto, "VALOR ACTUAL: %04lu", get_ldr());
	lcd_send_string(texto);
	lcd_put_cur(0, 3);
	lcd_send_string("VALOR NUEVO: ");
	sprintf(texto, "%c%04lu%c", 0x7F, valorUmbral, 0x7E);
	lcd_send_string(texto);
	pantallaUmbral = 0;
	timeOut_pantalla = 0;
} //fin init_LdrApaga()

/////////////////////////////////////////
//            ACCIONES                 //
/////////////////////////////////////////

void acc_Info (void){

	if (flag_infoDHT != 0){
		sensorDHT = get_datosDHT();
		temperatura = sensorDHT.temp;
		humedad = sensorDHT.hum;

		lcd_put_cur(6, 0);
		sprintf(texto, "%02d  ", temperatura);
		lcd_send_string(texto);
		lcd_put_cur(8, 0);
		lcd_send_customChar(2); //grados
		lcd_put_cur(9, 0);
		lcd_send_string("C");

		lcd_put_cur(9, 1);
		sprintf(texto, "%02d%%  ", humedad);
		lcd_send_string(texto);

		lcd_put_cur(5, 2);
		lcd_send_string( (getStat_rele() != 0) ? "APAGADA " : "PRENDIDA");
		lcd_put_cur(0, 3);
		lcd_send_string((get_modoLuz() != 0) ? "AUTOMATICO" : "MANUAL    ");

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

		lcd_put_cur(3, 2);
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

		lcd_put_cur(3, 2);
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
		menuActual = &menu[MENU_SELECCION];
		menuActual->inicia_menu();
	} //fin if IN_BACK

	if (getStatBoton(IN_LEFT) == FALL){
		if (aux_modoLuz != 0){
			aux_modoLuz = 0;
		}else{
			aux_modoLuz = 1;
		}

		lcd_put_cur(9, 2);
		switch (aux_modoLuz) {
			case 0:
				lcd_send_string("OFF");
			break;
			case 1:
				lcd_send_string("ON ");
			break;
			default:
			break;
		} //fin switch modoLuz
	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){
		if (aux_modoLuz != 0){
			aux_modoLuz = 0;
		}else{
			aux_modoLuz = 1;
		}

		lcd_put_cur(9, 2);
		switch (aux_modoLuz) {
			case 0:
				lcd_send_string("OFF");
			break;
			case 1:
				lcd_send_string("ON ");
			break;
			default:
			break;
		} //fin switch modoLuz
	} //fin if IN_RIGHT

	if (getStatBoton(IN_OK) == FALL){
		modoLuz = aux_modoLuz;
//		if (modoLuz != 0){
//			setOutput(OUT_MODO, 0); //logica negativa
//		}else{
//			setOutput(OUT_MODO, 1); //logica negativa
//		}
		set_modoLuz(modoLuz);

		menuActual = &menu[MENU_SELECCION];
		menuActual->inicia_menu();
	} //fin if IN_OK
} //fin acc_ModoLuz()


void acc_LdrPrende (void){
	switch (pantallaUmbral){
		case 0:
			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
				break;
			} //fin if IN_BACK

			switch (getStatBoton(IN_LEFT)) {
				case FALL:
					valorUmbral--;
					if (valorUmbral > 3000) valorUmbral = 3000;

					lcd_put_cur(14, 3);
					sprintf(texto, "%04lu", valorUmbral);
					lcd_send_string(texto);

					holdBoton = 0;
				break;
				case LOW_L:
					if (holdBoton > 150){ //en 10*ms.
						flag_holdBoton = 1;
					}

					if (flag_holdBoton != 0){
						if (repitePulso > 24){ //en 10*ms.
							valorUmbral -= 10;
							if (valorUmbral > 3000) valorUmbral = 3000;

							lcd_put_cur(14, 3);
							sprintf(texto, "%04lu", valorUmbral);
							lcd_send_string(texto);

							repitePulso = 0;
						} //fin if repitePulso
					} //fin if flag_holdBoton
				break;
				case RISE:
					flag_holdBoton = 0;
				break;
				default:
				break;
			} //fin switch IN_LEFT

			switch (getStatBoton(IN_RIGHT)) {
				case FALL:
					valorUmbral++;
					if (valorUmbral > 3000) valorUmbral = 0;

					lcd_put_cur(14, 3);
					sprintf(texto, "%04lu", valorUmbral);
					lcd_send_string(texto);

					holdBoton = 0;
				break;
				case LOW_L:
					if (holdBoton > 150){ //en 10*ms.
						flag_holdBoton = 1;
					}

					if (flag_holdBoton != 0){
						if (repitePulso > 24){ //en 10*ms.
							valorUmbral += 10;
							if (valorUmbral > 3000) valorUmbral = 0;

							lcd_put_cur(14, 3);
							sprintf(texto, "%04lu", valorUmbral);
							lcd_send_string(texto);

							repitePulso = 0;
						} //fin if repitePulso
					} //fin if flag_holdBoton
				break;
				case RISE:
					flag_holdBoton = 0;
				break;
				default:
				break;
			} //fin switch IN_RIGHT

			if (timeOut_pantalla > 99){ // un segundo paso
				lcd_put_cur(14, 2);
				sprintf(texto, "%04lu", get_ldr());
				lcd_send_string(texto);
				timeOut_pantalla = 0;
			} //fin if timeOut_pantalla

			if (getStatBoton(IN_OK) == FALL){
				set_umbralLDR(0, valorUmbral);

				lcd_clear();
				lcd_put_cur(0, 1);
				lcd_send_string("UMBRAL NOCHE GRABADO");
				pantallaUmbral = 1;
				timeOut_pantalla = 0;
				break;
			} //fin if IN_OK
		break;
		case 1:
			if (timeOut_pantalla > 349){ // 3,5 segundos pasaron
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
			} //fin if timeOut_pantalla

			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
			} //fin if IN_BACK
		break;
		default:
		break;
	} //fin switch pantallaUmbral
} //fin acc_LdrPrende()


void acc_LdrApaga (void){
	switch (pantallaUmbral){
		case 0:
			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
				break;
			} //fin if IN_BACK

			switch (getStatBoton(IN_LEFT)) {
				case FALL:
					valorUmbral--;
					if (valorUmbral > 3000) valorUmbral = 3000;

					lcd_put_cur(14, 3);
					sprintf(texto, "%04lu", valorUmbral);
					lcd_send_string(texto);

					holdBoton = 0;
				break;
				case LOW_L:
					if (holdBoton > 150){ //en 10*ms.
						flag_holdBoton = 1;
					}

					if (flag_holdBoton != 0){
						if (repitePulso > 24){ //en 10*ms.
							valorUmbral -= 10;
							if (valorUmbral > 3000) valorUmbral = 3000;

							lcd_put_cur(14, 3);
							sprintf(texto, "%04lu", valorUmbral);
							lcd_send_string(texto);

							repitePulso = 0;
						} //fin if repitePulso
					} //fin if flag_holdBoton
				break;
				case RISE:
					flag_holdBoton = 0;
				break;
				default:
				break;
			} //fin switch IN_LEFT

			switch (getStatBoton(IN_RIGHT)) {
				case FALL:
					valorUmbral++;
					if (valorUmbral > 3000) valorUmbral = 0;

					lcd_put_cur(14, 3);
					sprintf(texto, "%04lu", valorUmbral);
					lcd_send_string(texto);

					holdBoton = 0;
				break;
				case LOW_L:
					if (holdBoton > 150){ //en 10*ms.
						flag_holdBoton = 1;
					}

					if (flag_holdBoton != 0){
						if (repitePulso > 24){ //en 10*ms.
							valorUmbral += 10;
							if (valorUmbral > 3000) valorUmbral = 0;

							lcd_put_cur(14, 3);
							sprintf(texto, "%04lu", valorUmbral);
							lcd_send_string(texto);

							repitePulso = 0;
						} //fin if repitePulso
					} //fin if flag_holdBoton
				break;
				case RISE:
					flag_holdBoton = 0;
				break;
				default:
				break;
			} //fin switch IN_RIGHT

			if (timeOut_pantalla > 99){ // un segundo paso
				lcd_put_cur(14, 2);
				sprintf(texto, "%04lu", get_ldr());
				lcd_send_string(texto);
				timeOut_pantalla = 0;
			} //fin if timeOut_pantalla

			if (getStatBoton(IN_OK) == FALL){
				set_umbralLDR(1, valorUmbral);

				lcd_clear();
				lcd_put_cur(0, 1);
				lcd_send_string("UMBRAL DIA GRABADO");
				pantallaUmbral = 1;
				timeOut_pantalla = 0;
				break;
			} //fin if IN_OK
		break;
		case 1:
			if (timeOut_pantalla > 349){ // 3,5 segundos pasaron
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
			} //fin if timeOut_pantalla

			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
			} //fin if IN_BACK
		break;
		default:
		break;
	} //fin switch pantallaUmbral
} //fin acc_LdrApaga()


void refresh_infoDHT (void){
	flag_infoDHT = 1;
} //fin refresh_infoDHT()
