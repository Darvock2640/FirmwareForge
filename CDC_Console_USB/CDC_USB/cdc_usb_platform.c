/**
 * @file cdc_usb_platform.c
 * @brief Implementation of the CDC USB platform abstraction layer
 * 
 * This file contains the implementation of functions for initializing and
 * managing CDC USB communication on 32 bits Microchip microcontrollers. It 
 * provides complete USB CDC device functionality including event handling,
 * line-based command processing, and callback management.
 * 
 * @author Alejandro Beltran
 * @date September 2025
 */

#include "cdc_usb_platform.h"
#include <sys/types.h>

uint8_t CACHE_ALIGN cdcReadBuffer[APP_READ_BUFFER_SIZE];
uint8_t CACHE_ALIGN cdcWriteBuffer[APP_READ_BUFFER_SIZE];

char commandBuffer[APP_READ_BUFFER_SIZE];

static uint8_t command_index;
static void (*return_line_callback)(char*) = NULL;
static void (*console_ready_callback)(void) = NULL;

cdc_usb_t usbState = {
    /* Device Layer Handle  */
    .deviceHandle = USB_DEVICE_HANDLE_INVALID ,
    /* Device configured status */
    .isConfigured = false,
    /* Initial get line coding state */
    .getLineCodingData.dwDTERate = CDC_USB_GET_LINE_CODING_DTERATE,
    .getLineCodingData.bParityType = CDC_USB_GET_LINE_CODING_PARITY_TYPE,
    .getLineCodingData.bCharFormat= CDC_USB_GET_LINE_CODING_CHAR_FORMAT,
    .getLineCodingData.bDataBits = CDC_USB_GET_LINE_CODING_DATA_BITS,
    /* Read Transfer Handle */
    .readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,
    /* Write Transfer Handle */
    .writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID,
    /* Initialize the read complete flag */
    .isReadComplete = false,
    /*Initialize the write complete flag*/
    .isWriteComplete = true,
    /* Set up the read buffer */
    .cdcReadBuffer = &cdcReadBuffer[0],
    /* Set up the read buffer */
    .cdcWriteBuffer = &cdcWriteBuffer[0],
    /* Number of bytes read from Host */ 
    .numBytesRead = 0,
};

bool cdc_usb_initialize ( void )
{
    command_index = 0;                  // Initialize command index
    usbState.cdcReadBuffer[0] = '\0';   // Initialize the receive buffer
    usbState.cdcWriteBuffer[0] = '\0';  // Initialize the write buffer
    commandBuffer[0] = '\0';            // Initialize the command buffer
    if(usbState.isConfigured == true){
        usbState.readTransferHandle =  USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        USB_DEVICE_CDC_RESULT result;
        result = USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0,
                &usbState.readTransferHandle, usbState.cdcReadBuffer,
                APP_READ_BUFFER_SIZE);
        usbState.isReadComplete = false;
        if (result != USB_DEVICE_CDC_RESULT_OK)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

/***********************************************
 * Application USB Device Layer Event Handler.
 ***********************************************/
void APP_USBDeviceEventHandler 
(
    USB_DEVICE_EVENT event, 
    void * eventData, 
    uintptr_t context 
)
{
    USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData;

    switch(event)
    {
        case USB_DEVICE_EVENT_SOF:
            break;
        case USB_DEVICE_EVENT_RESET:
            usbState.isConfigured = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            if ( configuredEventData->configurationValue == 1)
            {
                /* Register the CDC Device application event handler here.
                 * Note how the usbState object pointer is passed as the
                 * user data */
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t)&usbState);
                /* Mark that the device is now configured */
                usbState.isConfigured = true;
                if(cdc_usb_initialize()){
                    cdc_usb_console_ready();
                }
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(usbState.deviceHandle);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not available. We can detach the device */
            USB_DEVICE_Detach(usbState.deviceHandle);
            usbState.isConfigured = false;
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
            break;
        case USB_DEVICE_EVENT_RESUMED:
            break;
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}

/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/
USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void * pData,
    uintptr_t userData
)
{
    cdc_usb_t * usbStateObject;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE * eventDataRead;

    usbStateObject = (cdc_usb_t *)userData;

    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */
            USB_DEVICE_ControlSend(usbStateObject->deviceHandle,
                    &usbStateObject->getLineCodingData, sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */
            USB_DEVICE_ControlReceive(usbStateObject->deviceHandle,
                    &usbStateObject->setLineCodingData, sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */
            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            usbStateObject->controlLineStateData.dtr = controlLineStateData->dtr;
            usbStateObject->controlLineStateData.carrier = controlLineStateData->carrier;
            USB_DEVICE_ControlStatus(usbStateObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */
            usbStateObject->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            /* Complete the control transfer by sending a ZLP  */
            USB_DEVICE_ControlStatus(usbStateObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            /* This means that the host has sent some data*/
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            if(eventDataRead->status != USB_DEVICE_CDC_RESULT_ERROR)
            {
                usbStateObject->isReadComplete = true;
                usbStateObject->numBytesRead = eventDataRead->length;
                cdc_usb_read_line();
            }
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */
            USB_DEVICE_ControlStatus(usbStateObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
            /* This means the GET LINE CODING function data is valid. We don't
             * do much with this data in this demo. */
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            /* This means that the data write got completed. We can schedule
             * the next read. */
            usbStateObject->isWriteComplete = true;
            break;
        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

cdc_usb_t* get_cdc_usb_handle(void){
    return &usbState;
}

void cdc_usb_read_line(void){
    if(usbState.numBytesRead == 0 || usbState.isReadComplete == false){
        return; // No data to process
    }
    char receivedBuffer[APP_READ_BUFFER_SIZE] = {0};
    // Process the received data
    for(uint16_t i = 0; i < usbState.numBytesRead; i++)
    {
        // Process termination
        if (usbState.cdcReadBuffer[i] == CDC_USB_LINE_TERMINATOR)
        {
            commandBuffer[command_index] = '\0'; // Null-terminate the command
            cdc_usb_return_line();
            break;
        }
        // Process reset characters
        else if (usbState.cdcReadBuffer[i] == CDC_USB_RESET_LINE_CHAR_1 || usbState.cdcReadBuffer[i] == CDC_USB_RESET_LINE_CHAR_2)
        {
            command_index = 0;
            sprintf(receivedBuffer, CDC_USB_RESET_LINE_RESPONSE);
            break;
        }
        // Regular character processing
        else
        {
            commandBuffer[command_index] = usbState.cdcReadBuffer[i];
            command_index++;
            receivedBuffer[i] = usbState.cdcReadBuffer[i];
        }
    }
    usbState.isReadComplete = false;
    usbState.numBytesRead = 0;
    USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0,
                    &usbState.readTransferHandle, usbState.cdcReadBuffer,
                    APP_READ_BUFFER_SIZE);
    cdc_usb_write(receivedBuffer);
}

void cdc_usb_return_line_callback_register(void (*callback)(char*)){
	// Register a callback function to be called when a line is returned
	return_line_callback = callback;
}

void cdc_usb_return_line_callback_unregister(void){
	// Unregister the callback function
	return_line_callback = NULL;
}

void cdc_usb_return_line(void){
	// Call the registered callback function with the command buffer
	if (return_line_callback != NULL) {
		return_line_callback(commandBuffer);
	}
	
	// Reset the command index and buffer for the next command
	command_index = 0;
    commandBuffer[0] = '\0';
}

bool cdc_usb_write(char *data){
    // Write data to the USB CDC interface
    if(data == NULL || data[0] == '\0'){
        return false;
    }
    if(usbState.isConfigured == false || usbState.isWriteComplete == false){
        return false;
    }
    sprintf((char* )usbState.cdcWriteBuffer, "%s", data);
    usbState.isWriteComplete = false;
    usbState.writeTransferHandle =  USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    USB_DEVICE_CDC_RESULT result;
    result = USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0,
                    &usbState.writeTransferHandle,
                    usbState.cdcWriteBuffer, strlen((char *)usbState.cdcWriteBuffer),
                    USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
    if (result != USB_DEVICE_CDC_RESULT_OK)
    {
        return false;
    }
    return true;
}

void cdc_usb_console_ready_callback_register(void (*callback)(void)){
	// Register a callback function to be called when the console is ready
	console_ready_callback = callback;
}

void cdc_usb_console_ready_callback_unregister(void){
	// Unregister the callback function
	console_ready_callback = NULL;
}

void cdc_usb_console_ready(void){
	// Call the registered callback function with the command buffer
	if (console_ready_callback != NULL) {
		console_ready_callback();
	}
}