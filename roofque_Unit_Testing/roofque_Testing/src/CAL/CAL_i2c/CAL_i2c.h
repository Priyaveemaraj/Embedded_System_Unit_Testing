/**
 * @file i2c.h
 * @brief Hardware abstraction wrapper for STM32 HAL I2C communication.
 *
 * This file provides a set of wrapper functions around the STM32 HAL I2C
 * driver to simplify communication with I2C devices. It includes functions
 * for register-based read and write operations using both blocking and
 * DMA-based methods.
 *
 * The interface allows:
 * - Initialization of the I2C peripheral
 * - Register-based read operations
 * - Register-based write operations
 * - DMA-based I2C transfers
 * - Simple delay wrapper using HAL
 *
 * These functions are intended to provide a clean and reusable interface
 * for interacting with I2C sensors and peripherals.
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

#ifndef I2C_CAL_I2C_H_
#define I2C_CAL_I2C_H_

#include "CAL/CAL.h"


#ifdef USE_CAL_I2C

extern I2C_HandleTypeDef hi2c2 ;

/**
 * @brief Initialize the I2C peripheral using the HAL driver.
 *
 * This function initializes the I2C peripheral by calling the
 * HAL_I2C_Init() function with the provided I2C handle.
 *
 * @param hi2c Pointer to the I2C handle structure that contains
 *             the configuration information for the specified I2C.
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_I2C_Init(I2C_HandleTypeDef* hi2c);

/**
 * @brief Write a single byte to a register of an I2C device.
 *
 * This function writes one byte of data to a specific register
 * address of an I2C slave device using the HAL memory write API.
 *
 * @param hi2c Pointer to the I2C handle structure.
 * @param device_address 7-bit I2C slave device address (shifted as required by HAL).
 * @param mem_address Register address inside the slave device to write to.
 * @param tx_buffer Pointer to the data byte that will be written to the device.
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_I2C_Write(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address, uint8_t *tx_buffer);

/**
 * @brief Read data from a register of an I2C device.
 *
 * This function reads one or more bytes from a specified register
 * address of an I2C slave device using the HAL memory read API.
 *
 * @param hi2c Pointer to the I2C handle structure.
 * @param device_address 7-bit I2C slave device address (shifted as required by HAL).
 * @param mem_address Register address inside the slave device to read from.
 * @param rx_buffer Pointer to the buffer where the received data will be stored.
 * @param rx_buffer_size Number of bytes to read from the device.
 *
 * @return CAL_status
 *         -  CAL_OK if the read operation is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_I2C_Read(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address, uint8_t* rx_buffer, uint16_t rx_buffer_size);

/**
 * @brief Write a single byte to an I2C device register using DMA.
 *
 * This function performs a non-blocking memory write operation
 * to a slave device register using DMA.
 *
 * @param hi2c Pointer to the I2C handle structure.
 * @param device_address 7-bit I2C slave device address (shifted as required by HAL).
 * @param mem_address Register address inside the slave device to write to.
 * @param tx_buffer Pointer to the data byte to be transmitted.
 *
 * @return CAL_status
 *         -  CAL_OK if the DMA transfer started successfully
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_I2C_Write_DMA(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address, uint8_t* tx_buffer);

/**
 * @brief Read data from an I2C device register using DMA.
 *
 * This function performs a non-blocking memory read operation
 * from a slave device register using DMA.
 *
 * @param hi2c Pointer to the I2C handle structure.
 * @param device_address 7-bit I2C slave device address (shifted as required by HAL).
 * @param mem_address Register address inside the slave device to read from.
 * @param rx_buffer Pointer to the buffer where received data will be stored.
 * @param rx_buffer_size Number of bytes to read.
 *
 * @return CAL_status
 *         -  CAL_OK if the DMA transfer started successfully
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_I2C_Read_DMA(I2C_HandleTypeDef* hi2c, uint16_t device_address, uint16_t mem_address,uint8_t* rx_buffer, uint16_t rx_buffer_size);

#endif

#endif /* I2C_CAL_I2C_H_ */
