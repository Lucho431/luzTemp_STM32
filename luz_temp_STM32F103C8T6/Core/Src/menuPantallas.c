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
#include "24lc256_lfs.h"
#include "hora_tablero.h"
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
uint8_t flag_infoModo = 0;
//variables menu seleccion
uint8_t cursor = 0;
//variables menu modo luz
uint8_t modoLuz;
//variables menu umbral
uint8_t pantallaUmbral = 0;
uint32_t valorUmbral;
//variables menu hora y fecha
RTC_TimeTypeDef muestraHora;
RTC_DateTypeDef muestraFecha;
uint8_t cursor_fechaHora = 0;
T_PANT_HORA_FECHA pant_horaFecha = PANT_HORA_FECHA;
uint8_t diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint8_t diaSemana; //de 1 a 7 -> lunes a domingo.
//variables menu debug
uint8_t cursor_debug = 0;
static uint8_t cuentaResets = 0;
T_PANT_DEBUG pantDebug = PANT_DEBUG;
HAL_StatusTypeDef validaLectura;
RTC_TimeTypeDef muestraHoraOff;
RTC_DateTypeDef muestraFechaOff;


//variables de botones
uint8_t holdBoton = 0;
uint8_t flag_holdBoton = 0;
uint8_t repitePulso = 0;
//variables externas
extern I2C_HandleTypeDef hi2c1;

void acc_Info (void);
void acc_Seleccion (void);
void acc_ModoLuz (void);
void acc_LdrPrende (void);
void acc_LdrApaga (void);
void acc_setHora (void);
void acc_debug (void);

void init_Info (void);
void init_Seleccion (void);
void init_ModoLuz (void);
void init_LdrPrende (void);
void init_LdrApaga (void);
void init_setHora (void);
void init_debug (void);

T_MENU menu[SIZE_MENU_NOMBRE] = {
		{MENU_INFO, NULL, init_Info, acc_Info}, //MENU_INFO,
		{MENU_SELECCION, NULL, init_Seleccion, acc_Seleccion}, //MENU_SELECCION,
		{MENU_MODO_LUZ, NULL, init_ModoLuz, acc_ModoLuz}, //MENU_MODO_LUZ,
		{MENU_LDR_PRENDE, NULL, init_LdrPrende, acc_LdrPrende}, //MENU_LDR_PRENDE,
		{MENU_LDR_APAGA, NULL, init_LdrApaga, acc_LdrApaga}, //MENU_LDR_APAGA,
		{MENU_SET_HORA, NULL, init_setHora, acc_setHora}, //MENU_SET_HORA
		{MENU_DEBUG, NULL, init_debug, acc_debug}, //MENU_DEBUG
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
//	temperatura = 27;
//	humedad = 75;

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
		case 3:
			lcd_put_cur(4, 2);
			lcd_send_string("HORA Y FECHA");
		break;
		case 4:
			lcd_put_cur(7, 2);
			lcd_send_string("DEBUG");
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
	modoLuz = get_modoLuz();
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


void init_setHora (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("AJUSTE HORA Y FECHA:");
	update_horaFecha();
	muestraHora = get_hora();
	muestraFecha = get_fecha();
	sprintf(texto, "%02d:%02d:%02d", muestraHora.Hours, muestraHora.Minutes, muestraHora.Seconds);
	lcd_put_cur(0, 2);
	lcd_send_string(texto);
	sprintf(texto, "%02d/%02d/%02d", muestraFecha.Date, muestraFecha.Month, muestraFecha.Year);
	lcd_put_cur(12, 2);
	lcd_send_string(texto);
	lcd_put_cur(9, 3);
	lcd_send_string("OK");

	cursor_fechaHora = 0;
//	lcd_put_cur(cursor_fechaHora, 1);
//	lcd_send_customChar(3); //arriba
//	lcd_put_cur(cursor_fechaHora, 3);
//	lcd_send_customChar(4); //abajo
	lcd_put_cur(cursor_fechaHora, 2);
	lcd_blinkCursOn;

	pant_horaFecha = PANT_HORA_FECHA;
} //init_setHora()


void init_debug (void){
	lcd_clear();
	lcd_put_cur(7, 0);
	lcd_send_string("DEBUG");
	lcd_put_cur(1, 2);
	lcd_send_data(0x7F); //<-
	validaLectura = HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &cuentaResets, 1, 100);
	switch (cursor_debug) {
		case 0:
			if (validaLectura != HAL_OK){
				lcd_send_string("  LOG RESET --  ");
			}else{
				sprintf(texto, "  LOG RESET %02d  ", cuentaResets);
				lcd_send_string(texto);
			}
		break;
		case 1:
			lcd_send_string("  CLEAR RESETS  ");
		break;
		case 2:
			lcd_send_string("  REG HORA OFF  ");
		break;
		case 3:
			lcd_send_string("TIEMPO ENCENDIDO");
		default:
		break;
	} //fin switch modoLuz
	lcd_send_data(0x7E); //->
	pantDebug = PANT_DEBUG;
} //fin init_debug()

/////////////////////////////////////////
//            ACCIONES                 //
/////////////////////////////////////////

void acc_Info (void){

	if (flag_infoDHT != 0){
		sensorDHT = get_datosDHT();
		temperatura = sensorDHT.temp;
		humedad = sensorDHT.hum;
//		temperatura = 27;
//		humedad = 75;

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
		flag_infoModo = 0;
	} //fin if flag_infoDHT

	if (flag_infoModo != 0){
		lcd_put_cur(5, 2);
		lcd_send_string( (getStat_rele() != 0) ? "APAGADA " : "PRENDIDA");
		lcd_put_cur(0, 3);
		lcd_send_string((get_modoLuz() != 0) ? "AUTOMATICO" : "MANUAL    ");

		flag_infoModo = 0;
	}

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
		if (cursor > 4) cursor = 4;

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
			case 3:
				lcd_send_string(" HORA Y FECHA ");
			break;
			case 4:
				lcd_send_string("    DEBUG     ");
			break;
			default:
			break;
		} //fin switch cursor
	} //fin if IN_LEFT

	if (getStatBoton(IN_RIGHT) == FALL){
		cursor++;
		if (cursor > 4) cursor = 0;

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
			case 3:
				lcd_send_string(" HORA Y FECHA ");
			break;
			case 4:
				lcd_send_string("    DEBUG     ");
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
			case 3:
				menuActual = &menu[MENU_SET_HORA];
			break;
			case 4:
				menuActual = &menu[MENU_DEBUG];
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
		if (modoLuz != 0){
			modoLuz = 0;
		}else{
			modoLuz = 1;
		}

		lcd_put_cur(9, 2);
		switch (modoLuz) {
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
		if (modoLuz != 0){
			modoLuz = 0;
		}else{
			modoLuz = 1;
		}

		lcd_put_cur(9, 2);
		switch (modoLuz) {
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


void acc_setHora (void){

	switch (pant_horaFecha){
		case PANT_HORA_FECHA:

			if (getStatBoton(IN_BACK) == FALL){
				lcd_blinkCursOff;
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
				return;
			}

			if (getStatBoton(IN_RIGHT) == FALL){

//				lcd_put_cur(cursor_fechaHora, 1);
//				lcd_send_string(" ");
//				lcd_put_cur(cursor_fechaHora, 3);
//				lcd_send_string(" ");

				switch (cursor_fechaHora) {
					case 0:
					case 3:
					case 6:
					case 12:
					case 15:
					case 18:
					case 19:
						cursor_fechaHora++;
					break;
					case 1:
					case 4:
					case 13:
					case 16:
						cursor_fechaHora += 2;
					break;
					case 9:
						cursor_fechaHora += 3;
					break;
					case 7:
						cursor_fechaHora += 5;
					break;
					case 20:
						cursor_fechaHora = 0;
					break;
					default:
					break;
				} //fin switch cursor_fechaHora

				if (cursor_fechaHora != 20){
					lcd_put_cur(cursor_fechaHora, 2);
				}else{
					lcd_put_cur(9, 3);
				} //fin if (cursor_fechaHora != 20)

			} //fin if IN_RIGHT

			if (getStatBoton(IN_LEFT) == FALL){

				switch (cursor_fechaHora) {
					case 0:
						cursor_fechaHora = 20;
					break;
					case 3:
					case 6:
					case 9:
					case 15:
					case 18:
						cursor_fechaHora -= 2;
					break;
					case 1:
					case 4:
					case 7:
					case 13:
					case 16:
					case 19:
					case 20:
						cursor_fechaHora--;
					break;
					case 12:
						cursor_fechaHora -= 5;
					break;
					default:
					break;
				} //fin switch cursor_fechaHora

				if (cursor_fechaHora != 20){
					lcd_put_cur(cursor_fechaHora, 2);
				}else{
					lcd_put_cur(9, 3);
				} //fin if (cursor_fechaHora != 20)

			} //fin if IN_LEFT

			if (getStatBoton(IN_OK) == FALL){
				switch (cursor_fechaHora) {
					case 0:
						muestraHora.Hours += 10;
						if (muestraHora.Hours > 23) muestraHora.Hours %= 10;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraHora.Hours);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 1:
						muestraHora.Hours++;
						if (muestraHora.Hours > 23) muestraHora.Hours = 0;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraHora.Hours);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 3:
						muestraHora.Minutes += 10;
						if (muestraHora.Minutes > 59) muestraHora.Minutes %= 10;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraHora.Minutes);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 4:
						muestraHora.Minutes++;
						if (muestraHora.Minutes > 59) muestraHora.Minutes = 0;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraHora.Minutes);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 6:
						muestraHora.Seconds += 10;
						if (muestraHora.Seconds > 59) muestraHora.Seconds %= 10;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraHora.Seconds);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 7:
						muestraHora.Seconds++;
						if (muestraHora.Seconds > 59) muestraHora.Seconds = 0;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraHora.Seconds);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 12:
						muestraFecha.Date += 10;
						if (muestraFecha.Date > 31) muestraFecha.Date %= 10;
						if (!muestraFecha.Date) muestraFecha.Date++;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraFecha.Date);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 13:
						muestraFecha.Date++;
						if (muestraFecha.Date > 31) muestraFecha.Date = 1;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraFecha.Date);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 15:
						muestraFecha.Month += 10;
						if (muestraFecha.Month > 12) muestraFecha.Month %= 10;
						if (!muestraFecha.Month) muestraFecha.Month++;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraFecha.Month);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 16:
						muestraFecha.Month++;
						if (muestraFecha.Month > 12) muestraFecha.Month = 1;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraFecha.Month);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 18:
						muestraFecha.Year += 10;
						if (muestraFecha.Year > 2099) muestraFecha.Year = 2000;

						lcd_put_cur(cursor_fechaHora, 2);
						sprintf(texto, "%02d", muestraFecha.Year%100);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 19:
						muestraFecha.Year++;
						if (muestraFecha.Year > 2099) muestraFecha.Year = 2000;

						lcd_put_cur(cursor_fechaHora-1, 2);
						sprintf(texto, "%02d", muestraFecha.Year%100);
						lcd_send_string(texto);
						lcd_put_cur(cursor_fechaHora, 2);
					break;
					case 20:
						lcd_blinkCursOff;
						if ( (muestraFecha.Year % 4 == 0 && muestraFecha.Year % 100 != 0) || (muestraFecha.Year % 400 == 0) ){
							diasPorMes[2] = 29;
						}else{
							diasPorMes[2] = 28;
						}

						if (muestraFecha.Date > diasPorMes[muestraFecha.Month]){
							lcd_clear();
							lcd_put_cur(3, 1);
							lcd_send_string("FECHA INVALIDA");
							timeOut_pantalla = 0;
							pant_horaFecha = PANT_ERROR_H_F;
						} //fin if (muestraFecha.Date > diasPorMes[muestraFecha.Month])

						diaSemana = calculaDiaSemana(muestraFecha.Year, muestraFecha.Month, muestraFecha.Date);

						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("CONF. HORA Y FECHA?");
						lcd_put_cur(0, 2);
						lcd_send_string("ATRAS:NO  ACEPTAR:SI");

						pant_horaFecha = PANT_CONFIR_H_F;

					break;
					default:
					break;
				} //fin switch cursor_fechaHora
			} //fin if IN_UP

		break;
		case PANT_CONFIR_H_F:
			if (getStatBoton(IN_BACK) == FALL){
				init_setHora();
				return;
			} //fin if IN_BACK

			if (getStatBoton(IN_OK) == FALL){
				HAL_RTC_SetTime(&hrtc, &muestraHora, RTC_FORMAT_BIN);
				HAL_RTC_SetDate(&hrtc, &muestraFecha, RTC_FORMAT_BIN);
				lcd_clear();
				lcd_put_cur(3, 1);
				lcd_send_string("AJUSTE GRABADO.");
				pant_horaFecha = PANT_GRABADO_H_F;
				timeOut_pantalla = 0;
				break;
			} //fin if IN_OK
		break;
		case PANT_GRABADO_H_F:
			if (timeOut_pantalla > 349){ // 3.5 segundos pasaron
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
				break;
			} //fin if timeOut...

			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_SELECCION];
				menuActual->inicia_menu();
			} //fin if IN_BACK

		break;
		case PANT_ERROR_H_F:
			if (timeOut_pantalla > 249){ // 2.5 segundos pasaron
				init_setHora();
				break;
			} //fin if timeOut...

			if (getStatBoton(IN_BACK) == FALL){
				init_setHora();
				break;
			} //fin if IN_BACK

		break;
		default:
		break;
	} //fin switch pant_horaFecha

} //acc_setHora()


void acc_debug (void){

	switch (pantDebug){
		case PANT_DEBUG:
			if (getStatBoton(IN_BACK) == FALL){
				menuActual = &menu[MENU_INFO];
				menuActual->inicia_menu();
			} //fin if IN_BACK

			if (getStatBoton(IN_LEFT) == FALL){
				cursor_debug--;
				if (cursor_debug > 3){
					cursor_debug = 3;
				}
				lcd_put_cur(2, 2);
				switch (cursor_debug) {
					case 0:
						validaLectura = HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &cuentaResets, 1, 100);
						if (validaLectura != HAL_OK){
							lcd_send_string("  LOG RESET --  ");
						}else{
							sprintf(texto, "  LOG RESET %02d  ", cuentaResets);
							lcd_send_string(texto);
						}
					break;
					case 1:
						lcd_send_string("  CLEAR RESETS  ");
					break;
					case 2:
						lcd_send_string("  REG HORA OFF  ");
					break;
					case 3:
						lcd_send_string("TIEMPO ENCENDIDO");
					break;
					default:
					break;
				} //fin switch modoLuz
			} //fin if LEFT

			if (getStatBoton(IN_RIGHT) == FALL){
				cursor_debug++;
				if (cursor_debug > 3){
					cursor_debug = 0;
				}
				lcd_put_cur(2, 2);
				switch (cursor_debug) {
					case 0:
						validaLectura = HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &cuentaResets, 1, 100);
						if (validaLectura != HAL_OK){
							lcd_send_string("  LOG RESET --  ");
						}else{
							sprintf(texto, "  LOG RESET %02d  ", cuentaResets);
							lcd_send_string(texto);
						}
					break;
					case 1:
						lcd_send_string("  CLEAR RESETS  ");
					break;
					case 2:
						lcd_send_string("  REG HORA OFF  ");
					break;
					case 3:
						lcd_send_string("TIEMPO ENCENDIDO");
					break;
					default:
					break;
				} //fin switch modoLuz
			} //fin if RIGHT

			if (getStatBoton(IN_OK) == FALL){
				switch (cursor_debug){
					case 1:
						lcd_clear();
						lcd_put_cur(0, 0);
						lcd_send_string("BORRAR LOG RESET?");
						lcd_put_cur(0, 2);
						lcd_send_string("ATRAS:NO  ACEPTAR:SI");
						pantDebug = PANT_BORRAR_RESETS;
					break;
					case 2:
						start_regHora();

						lcd_clear();
						lcd_put_cur(0, 1);
						lcd_send_string("INICIA REG. HORA OFF");
						update_horaFecha();
						muestraHora = get_hora();
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_HORA_ON, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraHora, 3, 100);
						HAL_Delay(10);
						muestraFecha = get_fecha();
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_FECHA_ON, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraFecha, 4, 100);
						HAL_Delay(10);
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_HORA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraHora, 3, 100);
						HAL_Delay(10);
						HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_FECHA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraFecha, 4, 100);
						pantDebug = PANT_REG_OFF;
						timeOut_pantalla = 0;
					break;
					case 3:
						HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_HORA_ON, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraHora, 3, 100);
						HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_FECHA_ON, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraFecha, 4, 100);
						HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_HORA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraHoraOff, 3, 100);
						HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_FECHA_OFF, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&muestraFechaOff, 4, 100);

						lcd_clear();
						lcd_put_cur(2, 1);
						sprintf(texto, "ON  %02d%02d%02d %02d:%02d", muestraFecha.Date, muestraFecha.Month, muestraFecha.Year, muestraHora.Hours, muestraHora.Minutes);
						lcd_send_string(texto);
						lcd_put_cur(2, 2);
						sprintf(texto, "OFF %02d%02d%02d %02d:%02d", muestraFechaOff.Date, muestraFechaOff.Month, muestraFechaOff.Year, muestraHoraOff.Hours, muestraHoraOff.Minutes);
						lcd_send_string(texto);

						pantDebug = PANT_TIEMPO_VIVO;
					default:
					break;
				} //fin switch cursor_debug
			} //fin if OK
		break;
		case PANT_BORRAR_RESETS:
			if (getStatBoton(IN_BACK) == FALL){
				init_debug();
				break;
			} //fin if BACK

			if (getStatBoton(IN_OK) == FALL){
				lcd_clear();
				lcd_put_cur(1, 1);
				lcd_send_string("LOG RESET BORRADO");
				cuentaResets = 0;
				HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &cuentaResets, 1, 100);
				pantDebug = PANT_RESET_BORRADOS;
				timeOut_pantalla = 0;
				break;
			} //fin if OK
		break;
		case PANT_RESET_BORRADOS:
			if (getStatBoton(IN_BACK) == FALL){
				init_debug();
				break;
			} //fin if BACK

			if (timeOut_pantalla > 249){ // 2.5 segundos pasaron
				init_debug();
				break;
			} //fin if timeOut_pantalla
		break;
		case PANT_REG_OFF:
			if (getStatBoton(IN_BACK) == FALL){
				init_debug();
				break;
			} //fin if BACK

			if (timeOut_pantalla > 249){ // 2.5 segundos pasaron
				init_debug();
				break;
			} //fin if timeOut_pantalla
		break;
		case PANT_TIEMPO_VIVO:
			if (getStatBoton(IN_BACK) == FALL){
				init_debug();
				break;
			} //fin if BACK
		break;
		default:
		break;
	} //fin switch pantDebug

} //fin acc_debug()


void refresh_infoDHT (void){
	flag_infoDHT = 1;
} //fin refresh_infoDHT()


void refresh_infoModo (void){
	flag_infoModo = 1;
} //fin refresh_infoModo


uint8_t calculaDiaSemana (int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }

    int k = year % 100;
    int j = year / 100;

    int h = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

    // Ajustamos el resultado para que el domingo sea el día 7.
	uint8_t d = (h + 6) % 7;

	if (!d) d = 7;

	return d;

} //fin calculoDiaSemana()
