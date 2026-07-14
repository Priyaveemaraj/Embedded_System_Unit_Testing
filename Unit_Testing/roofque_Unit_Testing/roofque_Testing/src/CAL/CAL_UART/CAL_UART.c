/**
 * @file    CAL_UART.c
 * @brief   UART Abstraction Layer implementation using STM32 HAL.
 *
 * @details
 * This file provides wrapper functions for UART communication
 * operations such as initialization, deinitialization, transmission,
 * and reception using polling, interrupt, and DMA modes.
 *
 * These functions abstract the STM32 HAL UART driver to provide
 * a consistent and simplified interface for higher-level modules.
 *
 * The module is part of the Common Abstraction Layer (CAL) and
 * isolates HAL dependencies from application and driver layers.
 *
 * Supported operations:
 *  - UART peripheral initialization and deinitialization
 *  - Blocking UART transmission
 *  - DMA-based UART transmission
 *  - Polling-based UART reception
 *  - Interrupt-based UART reception
 *  - DMA-based UART reception
 *  - UART reception using IDLE line detection
 *  - UART peripheral reset
 *
 * ---------------------------------------------------------------------------
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



#include "CAL_UART.h"

CAL_status CAL_UART_Init(UART_HandleTypeDef *huart)
{
    if (HAL_UART_Init(huart) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_DeInit(UART_HandleTypeDef *huart)
{
    if (HAL_UART_DeInit(huart) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Receive_DMA(UART_HandleTypeDef *huart,
                                uint8_t *pData,
                                uint16_t Size)
{
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(huart, pData, Size);
    if (status == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Receive_Poll(UART_HandleTypeDef *huart,
                                 uint8_t *pData,
                                 uint16_t Size)
{
    if (HAL_UART_Receive(huart, pData, Size, HAL_MAX_DELAY) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Receive_Idle_DMA(UART_HandleTypeDef *huart,
                                     uint8_t *pData,
                                     uint16_t Size)
{
    if (HAL_UARTEx_ReceiveToIdle_DMA(huart, pData, Size) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Receive_Idle_IT(UART_HandleTypeDef *huart,
                                    uint8_t *pData,
                                    uint16_t Size)
{
    if (HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Receive_Interrupt(UART_HandleTypeDef *huart,
                                      uint8_t *pData,
                                      uint16_t Size)
{
    if (HAL_UART_Receive_IT(huart, pData, Size) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Transmit(UART_HandleTypeDef *huart,
                             const uint8_t *pData,
                             uint16_t Size,
                             uint32_t Timeout)
{
    if (HAL_UART_Transmit(huart, pData, Size, Timeout) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Transmit_DMA(UART_HandleTypeDef *huart,
                                 const uint8_t *pData,
                                 uint16_t Size)
{
    if (HAL_UART_Transmit_DMA(huart, pData, Size) == HAL_OK) return CAL_OK;
    return CAL_ERROR;
}

CAL_status CAL_UART_Reset(UART_HandleTypeDef *huart)
{
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
        return CAL_ERROR;
    }

    HAL_Delay(100);

    if (HAL_UART_Init(huart) == HAL_OK) return CAL_OK;
    HAL_Delay(100);

    __HAL_UART_CLEAR_IDLEFLAG(huart);
    return CAL_ERROR;
}
