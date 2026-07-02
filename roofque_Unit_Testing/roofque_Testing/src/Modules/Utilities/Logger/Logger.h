/**
 * @file Logger.h
 * @brief Lightweight logging framework for embedded systems using CAL UART.
 *
 * This file provides a configurable logging interface designed for
 * embedded applications. The logger outputs formatted log messages
 * through the Common Abstraction Layer (CAL) UART driver.
 *
 * Features:
 * - Multiple log levels (DEBUG, INFO, WARNING, ERROR)
 * - Module based logging tags
 * - Variadic printf-style logging interface
 * - Optional RTOS compatible logging
 * - Configurable buffer size and timestamp support
 *
 * The logger is designed to provide a consistent and lightweight
 * debugging mechanism while keeping application code independent
 * from the underlying hardware communication layer.
 *
 * The logging system allows:
 * - Initialization of the logger with a UART interface
 * - Runtime control of the active log level
 * - Module based log message tagging
 * - Formatted log message printing using printf style arguments
 * - Optional RTOS safe transmission completion handling
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
 * Date         Author           Version     Description
 * ---------------------------------------------------------------------------
 * 14-03-2026   Prithiv Kanth    1.0.0       Initial implementation
 * ---------------------------------------------------------------------------
 */

#ifndef LOGGER_H
#define LOGGER_H



#ifdef __cplusplus
 extern "C" {
#endif




#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/* Include Common Abstraction Layer (CAL) */
#include "CAL/CAL.h"


//#define  USE_RTOS_LOGGER
// ==========================================
// CONFIGURATION SWITCHES
// ==========================================
/**
 * @brief Enable RTOS based logger operation.
 *
 * This macro controls whether the logger operates in
 * RTOS mode or bare-metal mode.
 *
 * Behaviour:
 *
 * If this macro is defined:
 *      #define USE_RTOS_LOGGER
 *  - Logger operates in RTOS mode
 *  - Log messages uses RTOS synchronization mechanisms
 *    (queues, semaphores, or task-safe transmission)
 *  - UART transmission completion is handled using
 *    log_tx_complete_callback()
 *
 * If this macro is NOT de#ifdef __cplusplus
 }
#endiffined (commented out):
 *      // #define USE_RTOS_LOGGER
 *  - Logger operates in bare-metal mode
 *  - No RTOS primitives are used
 *  - Logging is performed directly without task synchronization
 */



#define LOG_BUFFER_SIZE 120
#define LOG_TIMESTAMP_LEN 16


// ==========================================
// DEFAULT MODULE TAG
// Define this BEFORE including this header
// ==========================================

#ifndef LOG_MODULE
#define LOG_MODULE "SYS"
#endif


// ==========================================
// LOG LEVEL
// ==========================================

/**
 * @brief Log severity levels used by the logging system.
 *
 * This enumeration defines the priority levels used by the
 * logger to classify log messages. The active log level can
 * be configured at runtime to filter messages.
 *
 * Only messages with severity greater than or equal to the
 * configured level will be printed.
 */
typedef enum {
    LOG_LEVEL_DEBUG,    /**< Detailed debugging information */
    LOG_LEVEL_INFO,     /**< General runtime information */
    LOG_LEVEL_WARNING,  /**< Warning indicating potential issue */
    LOG_LEVEL_ERROR,    /**< Error indicating failure */
    LOG_LEVEL_NONE      /**< Disable all logging */
} LogLevel;


// ==========================================
// PUBLIC API
// ==========================================

/**
 * @brief Initialize the logging system.
 *
 * This function initializes the logger and binds it to a UART
 * interface through the CAL abstraction layer. After initialization,
 * all log messages will be transmitted through the configured UART.
 *
 * @param uart_handle UART interface handle used for log transmission.
 */
void log_init(CAL_UART_Handle_t uart_handle);


/**
 * @brief Set the active log level.
 *
 * This function configures the minimum severity level required
 * for a message to be printed. Messages with a lower priority
 * than the configured level will be ignored.
 *
 * @param level Desired log level threshold.
 */
void log_set_level(LogLevel level);


// ==========================================
// INTERNAL FUNCTIONS
// ==========================================

/**
 * @brief Print a debug level log message.
 *
 * This function formats and outputs a debug message including
 * the module tag and formatted string arguments.
 *
 * This function should normally not be called directly. Instead,
 * use the log_debug() macro which automatically inserts the
 * module tag.
 *
 * @param module Name of the module generating the log message.
 * @param format printf-style format string.
 * @param ... Variable arguments corresponding to the format string.
 */
void log_debug_ext(const char* module, const char* format, ...);


/**
 * @brief Print an informational log message.
 *
 * This function formats and outputs an informational message
 * including the module tag and formatted string arguments.
 *
 * @param module Name of the module generating the log message.
 * @param format printf-style format string.
 * @param ... Variable arguments corresponding to the format string.
 */
void log_info_ext(const char* module, const char* format, ...);


/**
 * @brief Print a warning log message.
 *
 * This function formats and outputs a warning message including
 * the module tag and formatted string arguments.
 *
 * @param module Name of the module generating the log message.
 * @param format printf-style format string.
 * @param ... Variable arguments corresponding to the format string.
 */
void log_warning_ext(const char* module, const char* format, ...);


/**
 * @brief Print an error log message.
 *
 * This function formats and outputs an error message including
 * the module tag and formatted string arguments.
 *
 * @param module Name of the module generating the log message.
 * @param format printf-style format string.
 * @param ... Variable arguments corresponding to the format string.
 */
void log_error_ext(const char* module, const char* format, ...);


void log_tx_complete_callback(void);

// ==========================================
// USER MACROS
// ==========================================

/**
 * @brief Print a debug log message using the default module tag.
 */
#define log_debug(...)   log_debug_ext(LOG_MODULE, __VA_ARGS__)

/**
 * @brief Print an informational log message using the default module tag.
 */
#define log_info(...)    log_info_ext(LOG_MODULE, __VA_ARGS__)

/**
 * @brief Print a warning log message using the default module tag.
 */
#define log_warning(...) log_warning_ext(LOG_MODULE, __VA_ARGS__)

/**
 * @brief Print an error log message using the default module tag.
 */
#define log_error(...)   log_error_ext(LOG_MODULE, __VA_ARGS__)


#ifdef USE_RTOS_LOGGER

/**
 * @brief Transmission completion callback for RTOS logging.
 *
 * This function should be called when a UART transmission
 * completes while using RTOS-based logging. It is typically
 * invoked from a UART DMA or interrupt completion callback.
 *
 * The logger uses this signal to release the next pending
 * log message from the queue.
 */
void log_tx_complete_callback(void);

#endif
#ifdef __cplusplus
 }
#endif
#endif
