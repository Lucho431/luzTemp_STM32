/*
 * 24LC256_lfs.c
 *
 *  Created on: 03 ago 2023
 *      Author: Luciano Salvatore
 */

#include <24lc256_lfs.h>

//variables
I2C_HandleTypeDef* hi2c;
uint8_t baseAddress; //0x50 lowest// change this according to ur setup


void mem_24LC256_init (I2C_HandleTypeDef* i2c_handler, uint8_t i2c_address){
	hi2c = i2c_handler;
	baseAddress = i2c_address << 1;
} //fin mem_24LC256_init()



void mem_24LC256_writePlan (uint8_t numPlan, uint8_t vectorPlan[]){

	if (numPlan > 64) return;

	uint8_t i2cDir = baseAddress;
	if (numPlan > 32){
		i2cDir++;
	}

	uint8_t buffI2cTx[66];
	uint16_t addrPlan = 1024*(numPlan - 1);

	buffI2cTx[0] = (addrPlan) >> 8;
	buffI2cTx[1] = (addrPlan & 0xFF);

	uint16_t bytesRestantes = 1024;
	uint16_t i = 0;

	while (bytesRestantes > 64){
		for (uint16_t j = 0; j < 64; j++){
			buffI2cTx[2 + j] = vectorPlan[(i + j)];
		} //fin for j

//		i2c_master_write_to_device(i2c_port, i2cDir, buffI2cTx, 66, pdMS_TO_TICKS(100) );
		HAL_I2C_Mem_Write(hi2c, i2cDir, addrPlan, I2C_MEMADD_SIZE_16BIT, buffI2cTx, 66, 100);
		//vTaskDelay(pdMS_TO_TICKS(10));
		HAL_Delay(10);
		bytesRestantes -= 64;
		i += 64;

		addrPlan += 64;
		buffI2cTx[0] = (addrPlan) >> 8;
		buffI2cTx[1] = (addrPlan & 0xFF);
	} //fin while (bytesRestantes...

	if (bytesRestantes != 0){
		for (uint8_t j = 0; j < bytesRestantes; j++){
			buffI2cTx[2 + j] = vectorPlan[i + j];
		} //fin for j
//		i2c_master_write_to_device(i2c_port, i2cDir, buffI2cTx, bytesRestantes, pdMS_TO_TICKS(1000) );
		HAL_I2C_Mem_Write(hi2c, i2cDir, addrPlan, I2C_MEMADD_SIZE_16BIT, buffI2cTx, bytesRestantes, 100);
//		vTaskDelay(pdMS_TO_TICKS(10));
		HAL_Delay(10);
	}

} //fin mem24LC256_writePlan()


void mem_24LC256_readIntervalo (uint8_t numPlan, uint8_t numInterv, uint8_t vectorInterv[]){

	if (numPlan > 64) return;
	if (numInterv > 50) return;

	uint8_t i2cDir = baseAddress;
	if (numPlan > 32){
		i2cDir++;
	}

	uint16_t addrInterv = 1024*(numPlan - 1) + 16*(numInterv - 1);
//	uint8_t vectorAddr[2] = { (addrInterv & 0xFF00) >> 8, (uint8_t)(addrInterv & 0xFF) };

//	i2c_master_write_read_device(i2c_port, i2cDir, vectorAddr, 2, vectorInterv, 16, pdMS_TO_TICKS(1000));
	HAL_I2C_Mem_Read(hi2c, i2cDir, addrInterv, I2C_MEMADD_SIZE_16BIT, vectorInterv, 16, 100);

} //fin mem_24LC256_readIntervalo()


void mem_24LC256_writeIntervalo (uint8_t numPlan, uint8_t numInterv, uint8_t vectorInterv[]){

	if (numPlan > 64) return;
	if (numInterv > 50) return;

	uint8_t i2cDir = baseAddress;
	if (numPlan > 32){
		i2cDir++;
	}

	uint8_t buffI2cTx[18];
	uint16_t addrInterv = 1024*(numPlan - 1) + 16*(numInterv - 1);

//	buffI2cTx[0] = (addrInterv) >> 8;
//	buffI2cTx[1] = (addrInterv & 0xFF);

	for (uint16_t j = 0; j < 16; j++){
		buffI2cTx[j] = vectorInterv[j];
	} //fin for j
//	i2c_master_write_to_device(i2c_port, i2cDir, buffI2cTx, 18, pdMS_TO_TICKS(100) );
	HAL_I2C_Mem_Write(hi2c, i2cDir, addrInterv, I2C_MEMADD_SIZE_16BIT, buffI2cTx, 16, 100);
//	vTaskDelay(pdMS_TO_TICKS(10));
	HAL_Delay(10);

} //fin mem_24LC256_writeIntervalo()
