/**
 * @file CAL_UART.h
 * @brief Hardware abstraction wrapper for STM32 HAL UART communication.
 *
 * This file provides a set of wrapper functions around the STM32 HAL UART
 * driver to simplify serial communication. It includes functions for UART
 * initialization, transmission, and reception using polling, interrupt,
 * and DMA-based methods.
 *
 * The interface allows:
 * - Initialization and deinitialization of the UART peripheral
 * - UART reset operation
 * - Blocking UART transmission
 * - DMA-based UART transmission
 * - Polling-based reception
 * - Interrupt-based reception
 * - DMA-based reception
 * - IDLE-line based reception for variable-length data
 *
 * These functions provide a reusable abstraction layer that separates
 * application logic from direct HAL driver calls.
 *
 * ---------------------------------------------------------------------------
 * Author           : Prithiv Kanth
 * Created Date     : 07-03-2026
 *
 * Last Modified By : Prithiv Kanth
 * Last Modified On : 14-03-2026
 *
 * Version          : 1.0.1
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author        Version     Description
 * ---------------------------------------------------------------------------
 * 07-03-2026   Prithiv Kanth  1.0.0       Initial implementation
 * 14-03-2026   Prithiv Kanth  1.0.1       Added UART_HandleTypedefs Alias
 * ---------------------------------------------------------------------------
 */

#ifndef PERIPHERALS_CAL_UART_H_
#define PERIPHERALS_CAL_UART_H_

#include "CAL/CAL.h"



extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart2;


#define CAL_UART_RxCpltCallback HAL_UART_RxCpltCallback
#define CAL_UART_ErrorCallback  HAL_UART_ErrorCallback

/**
 *
 * @brief UART handle abstraction type used by the CAL UART driver.
 *
 * This typedef defines a pointer to the STM32 HAL UART handle
 * (`UART_HandleTypeDef`). It is used within the CAL UART abstraction
 * layer to represent a UART interface instance.
 *
 * By using this alias, the CAL module can maintain a cleaner API and
 * loosely couple application code from the underlying HAL driver type.
 *
 * The handle contains configuration parameters, state information,
 * and hardware references required for UART communication.
 *
 */
typedef UART_HandleTypeDef* CAL_UART_Handle_t;

/**
 * @brief Initialize the UART peripheral using the HAL driver.
 *
 * This function initializes the UART peripheral by calling the
 * HAL_UART_Init() function using the provided UART handle.
 *
 * @param huart Pointer to the UART handle structure containing
 *              configuration parameters for the UART peripheral.
 */
CAL_status CAL_UART_Init(UART_HandleTypeDef *huart);

/**
 * @brief Deinitialize the UART peripheral.
 *
 * This function disables the UART peripheral and releases resources
 * associated with it using HAL_UART_DeInit().
 *
 * @param huart Pointer to the UART handle structure.
 */
CAL_status CAL_UART_DeInit(UART_HandleTypeDef *huart);

/**
 * @brief Reset the UART peripheral.
 *
 * This function performs a reset of the UART peripheral. It can be used
 * to recover from communication errors or unexpected states.
 *
 * @param huart Pointer to the UART handle structure.
 */
CAL_status CAL_UART_Reset(UART_HandleTypeDef *huart);


/**
 * @brief Transmit data over UART using blocking mode.
 *
 * This function transmits a specified number of bytes using the
 * HAL_UART_Transmit() API. The function blocks until transmission
 * is completed or the timeout expires.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the data buffer to transmit.
 * @param Size Number of bytes to transmit.
 * @param Timeout Maximum timeout duration for the transmission.
 */
CAL_status CAL_UART_Transmit(UART_HandleTypeDef *huart,
                       const uint8_t *pData,
                       uint16_t Size,
                       uint32_t Timeout);


/**
 * @brief Transmit data over UART using DMA.
 *
 * This function initiates a non-blocking UART transmission using DMA.
 * The CPU is free to execute other tasks while the DMA controller
 * transfers the data.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the transmit data buffer.
 * @param Size Number of bytes to transmit.
 */
CAL_status CAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                           const uint8_t *pData,
                           uint16_t Size);


/**
 * @brief Receive data using polling mode.
 *
 * This function reads a fixed number of bytes from the UART peripheral
 * using HAL_UART_Receive(). The function blocks until all requested
 * bytes are received.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the buffer where received data will be stored.
 * @param Size Number of bytes to receive.
 */
CAL_status CAL_UART_Receive_Poll(UART_HandleTypeDef *huart,
                           uint8_t *pData,
                           uint16_t Size);


/**
 * @brief Receive UART data using DMA.
 *
 * This function starts a non-blocking reception using DMA.
 * The received data will be stored in the provided buffer.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the buffer for received data.
 * @param Size Number of bytes to receive.
 */
CAL_status CAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                          uint8_t *pData,
                          uint16_t Size);


/**
 * @brief Receive UART data using interrupt mode with IDLE detection.
 *
 * This method allows reception of variable-length UART frames.
 * The reception ends when an IDLE line condition is detected.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the receive buffer.
 * @param Size Maximum buffer size for received data.
 */
CAL_status CAL_UART_Receive_Idle_IT(UART_HandleTypeDef *huart,
                              uint8_t *pData,
                              uint16_t Size);



/**
 * @brief Receive UART data using interrupt mode.
 *
 * This function enables UART reception using interrupts. The reception
 * process is non-blocking and completion is handled via UART callbacks.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the receive buffer.
 * @param Size Number of bytes to receive.
 */

CAL_status CAL_UART_Receive_Interrupt(UART_HandleTypeDef *huart,
                         uint8_t *pData,
                         uint16_t Size);

/**
 * @brief Receive UART data using DMA with IDLE detection.
 *
 * This function enables DMA-based UART reception and uses the
 * UART IDLE line interrupt to detect the end of a variable-length
 * message.
 *
 * @param huart Pointer to the UART handle structure.
 * @param pData Pointer to the receive buffer.
 * @param Size Maximum buffer size for received data.
 */
CAL_status CAL_UART_Receive_Idle_DMA(UART_HandleTypeDef *huart,
                               uint8_t *pData,
                               uint16_t Size);

#endif /* PERIPHERALS_CAL_UART_H_ */
