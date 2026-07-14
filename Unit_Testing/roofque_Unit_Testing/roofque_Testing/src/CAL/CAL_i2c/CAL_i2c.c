/**
 * @file i2c.h
 * @brief Hardware abstraction wrapper for STM32 HAL I2C communication.
 *
 * This file provides wrapper functions for STM32 HAL I2C operations
 * including blocking and DMA-based register read/write functions.
 *
 * ---------------------------------------------------------------------------
 * Author           : Aadhithya
 * Created Date     : 06-03-2026
 *
 * Last Modified By : Aadhithya
 * Last Modified On : 11-03-2026
 *
 * Version          : 1.0.1
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author        Version     Description
 * ---------------------------------------------------------------------------
 * 07-03-2026   Aadhithya     1.0.0       Initial implementation
 * 11-03-2026	Aadhithya	  1.0.1		  Added CAL_status return type
 * ---------------------------------------------------------------------------
 */

#include "CAL_i2c.h"

#ifdef USE_CAL_I2C

CAL_status CAL_I2C_Init(I2C_HandleTypeDef* hi2c){
	if (HAL_I2C_Init(hi2c) == HAL_OK){
		return CAL_OK;
	}else{
		return CAL_ERROR;
	}
}

CAL_status CAL_I2C_Write(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address,uint8_t *tx_buffer){
	if (HAL_I2C_Mem_Write(hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, tx_buffer, 1, HAL_MAX_DELAY) == HAL_OK){
		return CAL_OK;
	}else{
		return CAL_ERROR;
	}

}

CAL_status CAL_I2C_Read(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address,uint8_t* rx_buffer, uint16_t rx_buffer_size){
	if (HAL_I2C_Mem_Read(hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, rx_buffer, rx_buffer_size, HAL_MAX_DELAY) == HAL_OK){
		return CAL_OK;
	}else{
		return CAL_ERROR;
	}

}

CAL_status CAL_I2C_Write_DMA(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address, uint8_t* tx_buffer){
	if (HAL_I2C_Mem_Write_DMA(hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, tx_buffer, 1) == HAL_OK){
		return CAL_OK;
	}else{
		return CAL_ERROR;
	}
}

CAL_status CAL_I2C_Read_DMA(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address, uint8_t* rx_buffer, uint16_t rx_buffer_size){
	if (HAL_I2C_Mem_Read_DMA(hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, rx_buffer, rx_buffer_size) == HAL_OK){
		return CAL_OK;
	}else{
		return CAL_ERROR;
	}
}

#endif
