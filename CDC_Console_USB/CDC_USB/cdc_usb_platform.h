/**
 * @file cdc_usb_platform.h
 * @brief CDC USB platform abstraction layer header file
 * 
 * This header file provides a platform abstraction layer for CDC (Communication Device Class)
 * USB functionality on 32 bits Microchip microcontrollers. It implements a complete USB CDC interface
 * with line-based command processing, callback registration, and buffered I/O operations.
 * 
 * The module provides functionality for:
 * - USB CDC device initialization and configuration
 * - Line-based command input processing with echo
 * - Callback-driven command execution
 * - Buffered USB read/write operations
 * - USB device event handling
 * 
 * @author Alejandro Beltran
 * @date September 2025
 */

#ifndef _CDC_USB_PLATFORM_H
#define _CDC_USB_PLATFORM_H

#include "definitions.h"
#include "stdio.h"
#include "string.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

//! @brief Read buffer size for CDC USB operations
#define APP_READ_BUFFER_SIZE 512
//! @brief Line terminator character for command input
#define CDC_USB_LINE_TERMINATOR '\r'
//! @brief Reset line message
#define CDC_USB_RESET_LINE_RESPONSE "\n\rReset line\r\n"
//! @brief Reset line characters
#define CDC_USB_RESET_LINE_CHAR_1 7
#define CDC_USB_RESET_LINE_CHAR_2 8

/* Initial get line coding state */
//! @brief Default Data Terminal Rate (baud rate) for CDC USB
#define CDC_USB_GET_LINE_CODING_DTERATE 9600
//! @brief Default Parity Type for CDC USB
#define CDC_USB_GET_LINE_CODING_PARITY_TYPE 0
//! @brief Default Character Format for CDC USB
#define CDC_USB_GET_LINE_CODING_CHAR_FORMAT 0
//! @brief Default Data Bits for CDC USB
#define CDC_USB_GET_LINE_CODING_DATA_BITS 8


/**
 * @brief CDC USB application state structure
 * 
 * This structure contains all the state information required for managing
 * the CDC USB interface, including device handles, transfer states, buffers,
 * and configuration data.
 */
typedef struct
{
    /** @brief Device configured state - true when USB device is properly configured */
    bool isConfigured;
    /** @brief Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE deviceHandle;
    /** @brief Set Line Coding Data - configuration sent from host */
    USB_CDC_LINE_CODING setLineCodingData;
    /** @brief Get Line Coding Data - configuration sent to host */
    USB_CDC_LINE_CODING getLineCodingData;
    /** @brief Control Line State - DTR and carrier signals */
    USB_CDC_CONTROL_LINE_STATE controlLineStateData;
    /** @brief Read transfer handle for asynchronous read operations */
    USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle;
    /** @brief Write transfer handle for asynchronous write operations */
    USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle;
    /** @brief True when a read operation has completed */
    bool isReadComplete;
    /** @brief True when a write operation has completed */
    bool isWriteComplete;
    /** @brief Break data duration received from host */
    uint16_t breakData;
    /** @brief Pointer to application CDC read buffer */
    uint8_t * cdcReadBuffer;
    /** @brief Pointer to application CDC write buffer */
    uint8_t * cdcWriteBuffer;
    /** @brief Number of bytes read from Host in last operation */ 
    uint32_t numBytesRead; 
} cdc_usb_t;

/**
 * @brief Initializes the CDC USB platform
 * 
 * This function initializes the CDC USB state structure, clears all buffers,
 * and sets up the initial read transfer if the device is already configured.
 * Should be called during application startup after USB device initialization.
 * 
 * @return true if initialization was successful (device configured and read transfer started)
 * @return false if initialization failed or device is not yet configured
 */
bool cdc_usb_initialize ( void );

/**
 * @brief USB Device Layer Event Handler
 * 
 * This function handles USB device-level events such as configuration,
 * power detection, reset, suspend, and resume. It manages the overall
 * USB device state and registers the CDC-specific event handler when
 * the device becomes configured.
 * 
 * @param event The USB device event that occurred
 * @param eventData Pointer to event-specific data (may be NULL)
 * @param context User-defined context passed during registration
 */
void APP_USBDeviceEventHandler 
( 
    USB_DEVICE_EVENT event, 
    void * eventData, 
    uintptr_t context 
);

/**
 * @brief USB CDC Device Event Handler
 * 
 * This function handles CDC-specific USB events including line coding requests,
 * control line state changes, break signals, and data transfer completion.
 * It processes incoming data and manages the CDC communication protocol.
 * 
 * @param index CDC interface index (typically 0)
 * @param event The CDC event that occurred
 * @param pData Pointer to event-specific data
 * @param userData User data passed during handler registration (cdc_usb_t pointer)
 * @return USB_DEVICE_CDC_EVENT_RESPONSE_NONE (no specific response required)
 */
USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler 
( 
    USB_DEVICE_CDC_INDEX index, 
    USB_DEVICE_CDC_EVENT event, 
    void * pData, 
    uintptr_t userData 
);

/**
 * @brief Gets a pointer to the CDC USB state structure
 * 
 * This function provides access to the internal CDC USB state structure,
 * allowing external modules to monitor connection status, transfer states,
 * and other operational parameters.
 * 
 * @return Pointer to the global cdc_usb_t state structure
 */
cdc_usb_t* get_cdc_usb_handle(void);

/**
 * @brief Writes a null-terminated string to the CDC USB interface
 *
 * This function transmits a string over the USB CDC interface. It validates
 * the input data and device state, then copies the data to the internal write
 * buffer and initiates an asynchronous USB transfer. The function checks that
 * the device is configured and no other write operation is currently in progress.
 * 
 * If the device is not configured or another write operation is already in
 * progress, the function returns false immediately without blocking.
 * 
 * @param data The null-terminated string to transmit (must not be NULL or empty)
 * @return true if the write operation was successfully initiated
 * @return false if the operation failed (NULL/empty data, device not configured, 
 *               write in progress, or USB transfer error)
 * 
 * @note This function does NOT block - it returns false if a write is already in progress
 * @note The function performs internal buffering using sprintf
 * @note The write operation is asynchronous - completion is signaled via USB events
 * @note Check the return value to ensure the write was successfully initiated
 */
bool cdc_usb_write(char* data);

/**
 * @brief Processes received data for line-based input
 *
 * This function handles character-by-character processing of received USB data
 * to build complete command lines. It processes special characters and manages
 * the command buffer:
 * 
 * - Line terminator (CDC_USB_LINE_TERMINATOR): Triggers line completion and callback execution
 * - Reset characters (CDC_USB_RESET_LINE_CHAR_1/2): Resets the current command buffer and sends reset message
 * - All other characters: Added to command buffer and echoed back to host
 * 
 * The function automatically sets up the next USB read operation after processing
 * the current data. It should be called from the CDC event handler when read
 * completion events occur.
 * 
 * @note This function is called internally by the CDC event handler
 * @note Characters are echoed back to provide visual feedback to the user
 * @note The function handles buffer management and sets up the next read transfer
 * @note Reset characters trigger sending of CDC_USB_RESET_LINE_RESPONSE
 */
void cdc_usb_read_line(void);

/**
 * @brief Registers a callback function for complete line reception
 *
 * This function allows the application to register a callback that will be
 * executed whenever a complete command line is received (terminated by the
 * line terminator character). The callback receives the complete command string
 * as a parameter, allowing the application to process commands as they are received.
 * 
 * Only one callback can be registered at a time. Registering a new callback
 * will replace any previously registered callback. The callback function
 * should process the command quickly to avoid impacting USB operations.
 *
 * @param callback Function pointer to the callback function that accepts a char* parameter.
 *                 The callback receives the null-terminated command string.
 *                 Pass NULL to unregister the current callback.
 * 
 * @note The callback is executed from the USB event handler context
 * @note The command string passed to the callback is valid only during the callback execution
 * @note If more complex processing is needed, copy the command to a local buffer
 * @note The command buffer is automatically cleared after callback execution
 * 
 * @see cdc_usb_return_line_callback_unregister()
 */
void cdc_usb_return_line_callback_register(void (*callback)(char*));

/**
 * @brief Unregisters the currently registered line callback function
 *
 * This function removes the previously registered callback function, if any.
 * After calling this function, no callback will be executed when a complete
 * line is received. The function is safe to call even if no callback is
 * currently registered.
 * 
 * @note This is equivalent to calling cdc_usb_return_line_callback_register(NULL)
 * @see cdc_usb_return_line_callback_register()
 */
void cdc_usb_return_line_callback_unregister(void);

/**
 * @brief Executes the registered line callback and resets the command buffer
 *
 * This function is called internally when a complete line is received (when
 * a carriage return character is processed). It performs the following actions:
 * 
 * 1. Executes the registered callback function (if any) with the current command buffer
 * 2. Resets the command index to 0
 * 3. Clears the entire command buffer for the next command
 * 
 * This function should not be called directly by user code - it is automatically
 * invoked by the line processing logic in cdc_usb_read_line().
 * 
 * @note This function is for internal use only
 * @note The command buffer is automatically cleared after callback execution
 * @see cdc_usb_read_line()
 */
void cdc_usb_return_line(void);

/**
 * @brief Registers a callback function for console ready notification
 *
 * This function allows the application to register a callback that will be
 * executed when the CDC USB console becomes ready for operation. The console
 * is considered ready when the USB device is configured and the initial read
 * transfer has been successfully established.
 * 
 * This callback is particularly useful for applications that need to display
 * a welcome message or menu when the USB connection is established and ready
 * for user interaction.
 *
 * @param callback Function pointer to the callback function that takes no parameters.
 *                 Pass NULL to unregister the current callback.
 * 
 * @note Only one console ready callback can be registered at a time
 * @note The callback is executed when the USB device transitions to ready state
 * @note This callback is called from the USB event handler context
 * 
 * @see cdc_usb_console_ready_callback_unregister()
 * @see cdc_usb_console_ready()
 */
void cdc_usb_console_ready_callback_register(void (*callback)(void));

/**
 * @brief Unregisters the currently registered console ready callback function
 *
 * This function removes the previously registered console ready callback function,
 * if any. After calling this function, no callback will be executed when the
 * console becomes ready. The function is safe to call even if no callback is
 * currently registered.
 * 
 * @note This is equivalent to calling cdc_usb_console_ready_callback_register(NULL)
 * @see cdc_usb_console_ready_callback_register()
 */
void cdc_usb_console_ready_callback_unregister(void);

/**
 * @brief Executes the registered console ready callback
 *
 * This function is called internally when the CDC USB console becomes ready
 * for operation. It executes the registered console ready callback function
 * (if any) to notify the application that the USB CDC interface is now
 * available for communication.
 * 
 * The console is considered ready when:
 * - The USB device has been configured by the host
 * - The CDC interface has been properly initialized
 * - The initial read transfer has been established
 * 
 * This function should not be called directly by user code - it is automatically
 * invoked by the USB device event handler when the console becomes operational.
 * 
 * @note This function is for internal use only
 * @note Called from USB device event handler context
 * @see APP_USBDeviceEventHandler()
 */
void cdc_usb_console_ready(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _CDC_USB_PLATFORM_H */