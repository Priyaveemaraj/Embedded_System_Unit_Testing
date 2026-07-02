/**
 * @file CAL_CAN.h
 * @brief Implementation of the CAN abstraction layer for STM32 peripherals.
 *
 * This file implements helper functions for controlling CAN communication
 * using STM32 HAL CAN peripherals. The functions provided here
 * simplify CAN management by wrapping HAL APIs and directly
 * manipulating CAN registers when required.
 *
 * The implementation supports:
 * - Starting and stopping the CAN peripheral
 * - Configuring hardware filters for message filtering
 * - Activating interrupt-based notifications
 * - Adding messages to Tx mailboxes
 * - Retrieving messages from Rx FIFOs
 *
 * These utilities allow higher-level modules to control CAN communication
 * without directly interacting with low-level peripheral registers or
 * complex HAL structures.
 *
 * ---------------------------------------------------------------------------
 * Author           : Harish karthick R
 * Created Date     : 15-03-2026
 *
 * Last Modified By : Harish karthick R
 * Last Modified On : 15-03-2026
 *
 * Version          : 1.0.0
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author               Version     Description
 * ---------------------------------------------------------------------------
 * 15-03-2026   Harish karthick R    1.0.0       Initial implementation
 * ---------------------------------------------------------------------------
 */




#ifndef CAL_CAN__

#define CAL_CAN__

#ifdef __cplusplus
extern "C" {
#endif

#include "CAL/CAL.h"

#ifdef USE_CAL_CAN

// To extend the peripheral handles


extern CAN_HandleTypeDef hcan1;


//CAN callbacks

#define  CAL_CAN_RxFifo0MsgPendingCallback HAL_CAN_RxFifo0MsgPendingCallback

/**
 * @brief  Starts the CAN peripheral.
 * @note   This function transitions the CAN handle from the READY state to the BUSY state.
 * @param  hcan: Pointer to a CAN_HandleTypeDef structure that contains
 * the configuration information for the specified CAN.
 * @retval CAL_status: CAL_OK if the peripheral started successfully, CAL_ERROR otherwise.
 */
CAL_status CAL_CAN_Start(CAN_HandleTypeDef *hcan);

/**
 * @brief  Configures the CAN reception filter according to the specified parameters.
 * @param  hcan: Pointer to a CAN_HandleTypeDef structure.
 * @param  sFilterConfig: Pointer to a CAN_FilterTypeDef structure containing filter configuration.
 * @retval CAL_status: CAL_OK if configuration is successful, CAL_ERROR otherwise.
 */
CAL_status CAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, const CAN_FilterTypeDef *sFilterConfig);

/**
 * @brief  Enables the specified CAN interrupts (Notifications).
 * @param  hcan: Pointer to a CAN_HandleTypeDef structure.
 * @param  ActiveITs: Indicates which interrupts to enable.
 * Can be a combination of CAN_IT_TX_MAILBOX_EMPTY, CAN_IT_RX_FIFO0_MSG_PENDING, etc.
 * @retval CAL_status: CAL_OK if notifications were activated, CAL_ERROR otherwise.
 */
CAL_status CAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs);

/**
 * @brief  Adds a message to the first free Tx mailbox and requests transmission.
 * @param  hcan: Pointer to a CAN_HandleTypeDef structure.
 * @param  pHeader: Pointer to a CAN_TxHeaderTypeDef structure containing message details.
 * @param  aData: Array containing the payload to be transmitted.
 * @param  pTxMailbox: Pointer to a variable where the assigned Tx mailbox will be stored.
 * @retval CAL_status: CAL_OK if message was added to mailbox, CAL_ERROR otherwise.
 */
CAL_status CAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader, const uint8_t aData[], uint32_t *pTxMailbox);

/**
 * @brief  Retrieves a CAN frame from the specified Rx FIFO.
 * @param  hcan: Pointer to a CAN_HandleTypeDef structure.
 * @param  RxFifo: FIFO to read from (e.g., CAN_RX_FIFO0 or CAN_RX_FIFO1).
 * @param  pHeader: Pointer to a CAN_RxHeaderTypeDef structure to store received message details.
 * @param  aData: Array where the received payload will be stored.
 * @retval CAL_status: CAL_OK if a message was successfully retrieved, CAL_ERROR otherwise.
 */
CAL_status CAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);

#ifdef __cplusplus
}

#endif
#endif
#endif

