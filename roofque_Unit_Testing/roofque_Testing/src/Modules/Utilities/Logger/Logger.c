/**
 * @file    Logger.c
 * @brief   Embedded logging framework implementation using CAL UART.
 *
 * @details
 * This file implements a lightweight logging framework designed for
 * embedded systems. The logger provides formatted debug output through
 * the Common Abstraction Layer (CAL) UART interface.
 *
 * The module supports both **Bare-metal** and **RTOS-based** logging
 * depending on the configuration switch defined in `CAL_LOGGER.h`.
 *
 * The logger allows application modules to print formatted log messages
 * with severity levels and module tags, improving debugging and system
 * observability during development and runtime diagnostics.
 *
 * Each log message follows a structured format:
 *
 *      [timestamp][level][module] message
 *
 * Example output:
 *
 *      [00:00:12.532][INF][SYS] System initialized
 *      [00:00:15.105][WRN][MOTOR] Current limit reached
 *      [00:00:16.220][ERR][UART] Transmission timeout
 *
 * ---------------------------------------------------------------------------
 * LOGGER MODES
 * ---------------------------------------------------------------------------
 *
 * 1) Bare-metal Mode
 * ------------------
 * When `USE_RTOS_LOGGER` is NOT defined:
 *
 *  - Log messages are formatted in a static buffer
 *  - Transmission occurs immediately using blocking UART
 *  - No RTOS synchronization primitives are used
 *
 *
 * 2) RTOS Mode
 * -------------
 * When `USE_RTOS_LOGGER` is defined:
 *
 *  - Log messages are dynamically allocated
 *  - Messages are pushed into a FreeRTOS queue
 *  - A dedicated logger task transmits messages asynchronously
 *  - UART DMA is used for non-blocking transmission
 *  - A semaphore synchronizes DMA completion
 *
 *
 * ---------------------------------------------------------------------------
 * MODULE RESPONSIBILITIES
 * ---------------------------------------------------------------------------
 *
 * This module provides:
 *
 *  - Logger initialization
 *  - Log level configuration
 *  - Timestamp generation
 *  - Message formatting using printf-style arguments
 *  - Structured log output
 *  - Optional RTOS-safe logging via queues and tasks
 *
 *
 * ---------------------------------------------------------------------------
 * DEPENDENCIES
 * ---------------------------------------------------------------------------
 *
 *  - Common Abstraction Layer (CAL)
 *  - CAL UART driver
 *  - Standard C libraries (stdio, string, stdarg)
 *  - FreeRTOS (optional when RTOS logger enabled)
 *
 *
 * ---------------------------------------------------------------------------
 * Author           : Prithiv Kanth
 * Created Date     : 14-03-2026
 *
 * Last Modified By : Prithiv Kanth
 * Last Modified On : 14-03-2026
 *
 * Version          : 1.0.0
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author        Version     Description
 * ---------------------------------------------------------------------------
 * 14-03-2026   Prithiv Kanth  1.0.0       Initial implementation
 * ---------------------------------------------------------------------------
 */

#include "Logger.h"
#include <stdio.h>
#include <string.h>

// ==========================================
// RTOS DEPENDENCIES
// ==========================================


#ifdef USE_RTOS_LOGGER

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#define LOG_QUEUE_SIZE 20

static QueueHandle_t log_queue;
static SemaphoreHandle_t uart_tx_sema;
static TaskHandle_t log_task_handle;

#else
static char baremetal_log_buffer[LOG_BUFFER_SIZE];
#endif

// ==========================================
// STATIC VARIABLES
// ==========================================
static LogLevel current_log_level = LOG_LEVEL_INFO;
static CAL_UART_Handle_t log_uart = NULL;
static const char* LOG_LINE_ENDING = "\r\n";

// ==========================================
// HELPER FUNCTIONS
// ==========================================
static const char* get_level_prefix(LogLevel level)
{
    switch(level)
    {
        case LOG_LEVEL_DEBUG:   return "DBG";
        case LOG_LEVEL_INFO:    return "INF";
        case LOG_LEVEL_WARNING: return "WRN";
        case LOG_LEVEL_ERROR:   return "ERR";
        default:                return "UNK";
    }
}

static void get_timestamp(char* time_str, size_t max_len)
{
    uint32_t ms = CAL_GetTimerTicks();
    uint32_t total_seconds = ms / 1000;

    snprintf(time_str, max_len,
             "%02lu:%02lu:%02lu.%03lu",
             (total_seconds / 3600),
             ((total_seconds % 3600) / 60),
             (total_seconds % 60),
             (ms % 1000));
}

// ==========================================
// RTOS LOGGER IMPLEMENTATION
// ==========================================
#ifdef USE_RTOS_LOGGER

static void LogTask(void *pvParameters)
{
    char *msg_ptr;
    for(;;)
    {
        if (xQueueReceive(log_queue, &msg_ptr, portMAX_DELAY) == pdTRUE)
        {
            CAL_UART_Transmit_DMA(log_uart, (const uint8_t*)msg_ptr, strlen(msg_ptr));
            xSemaphoreTake(uart_tx_sema, portMAX_DELAY);
            vPortFree(msg_ptr);
        }
    }
}

void log_init(CAL_UART_Handle_t uart_handle)
{
    log_uart = uart_handle;
    log_queue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(char*));
    uart_tx_sema = xSemaphoreCreateBinary();

    xTaskCreate(LogTask, "LoggerTask", 256, NULL, tskIDLE_PRIORITY + 1, &log_task_handle);
}

static void log_output(LogLevel level, const char* module, const char* format, va_list args)
{
    if (level < current_log_level || log_uart == NULL || !log_queue) return;

    char *dyn_buffer = (char*)pvPortMalloc(LOG_BUFFER_SIZE);
    if (!dyn_buffer) return;

    char time_str[LOG_TIMESTAMP_LEN];
    get_timestamp(time_str, sizeof(time_str));

    int pos = snprintf(dyn_buffer, LOG_BUFFER_SIZE, "[%s][%s][%s] ",
                       time_str, get_level_prefix(level), module);

    if (pos > 0 && pos < LOG_BUFFER_SIZE)
    {
        int msg_len = vsnprintf(dyn_buffer + pos, LOG_BUFFER_SIZE - pos, format, args);
        if (msg_len > 0)
        {
            pos += msg_len;
            if (pos >= LOG_BUFFER_SIZE - 2) pos = LOG_BUFFER_SIZE - 3;
            strcpy(dyn_buffer + pos, LOG_LINE_ENDING);

            if (xQueueSend(log_queue, &dyn_buffer, 0) != pdPASS) vPortFree(dyn_buffer);
            return;
        }
    }
    vPortFree(dyn_buffer);
}

void log_tx_complete_callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(uart_tx_sema, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#else
// ==========================================
// BAREMETAL LOGGER
// ==========================================
void log_init(CAL_UART_Handle_t uart_handle) { log_uart = uart_handle; }

static void log_output(LogLevel level, const char* module, const char* format, va_list args)
{
    if (level < current_log_level || log_uart == NULL) return;

    char time_str[LOG_TIMESTAMP_LEN];
    get_timestamp(time_str, sizeof(time_str));

    int pos = snprintf(baremetal_log_buffer, LOG_BUFFER_SIZE, "[%s][%s][%s] ",
                       time_str, get_level_prefix(level), module);

    if (pos > 0 && pos < LOG_BUFFER_SIZE)
    {
        int msg_len = vsnprintf(baremetal_log_buffer + pos, LOG_BUFFER_SIZE - pos, format, args);
        if (msg_len > 0)
        {
            pos += msg_len;
            if (pos >= LOG_BUFFER_SIZE - 2) pos = LOG_BUFFER_SIZE - 3;
            strcpy(baremetal_log_buffer + pos, LOG_LINE_ENDING);

            CAL_UART_Transmit(log_uart, (const uint8_t*)baremetal_log_buffer,
                              strlen(baremetal_log_buffer), 0xFFFFFFFF);
        }
    }
}
#endif

// ==========================================
// PUBLIC WRAPPERS
// ==========================================
void log_set_level(LogLevel level) { current_log_level = level; }

void log_debug_ext(const char* module, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_output(LOG_LEVEL_DEBUG, module, format, args);
    va_end(args);
}

void log_info_ext(const char* module, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_output(LOG_LEVEL_INFO, module, format, args);
    va_end(args);
}

void log_warning_ext(const char* module, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_output(LOG_LEVEL_WARNING, module, format, args);
    va_end(args);
}

void log_error_ext(const char* module, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_output(LOG_LEVEL_ERROR, module, format, args);
    va_end(args);
}
