/**
 * @file CAL_CAN.c
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

#include "CAL_CAN.h"


#ifdef USE_CAL_CAN


CAL_status CAL_CAN_Start(CAN_HandleTypeDef *hcan){

	if (HAL_CAN_Start(hcan) != HAL_OK){
		return CAL_ERROR;
	}
	return CAL_OK;
}


CAL_status CAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, const CAN_FilterTypeDef *sFilterConfig){
	if (HAL_CAN_ConfigFilter(hcan, sFilterConfig) != HAL_OK){
		return CAL_ERROR;
	}
	return CAL_OK;
}


CAL_status CAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs){

	if (HAL_CAN_ActivateNotification(hcan, ActiveITs) != HAL_OK){
		return CAL_ERROR;
	}
	return CAL_OK;

}


CAL_status CAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader,const uint8_t aData[], uint32_t *pTxMailbox){
	if (HAL_CAN_AddTxMessage(hcan, pHeader, aData, pTxMailbox) != HAL_OK){
		return CAL_ERROR;
	}
	return CAL_OK;
}


CAL_status CAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo,CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]){
	if (HAL_CAN_GetRxMessage(hcan, RxFifo, pHeader, aData) != HAL_OK){
		return CAL_ERROR;
	}
	return CAL_OK;
}

#endif

