# CDC Console USB Example Documentation

## Overview

The CDC Console USB example demonstrates how to implement a USB Communication Device Class (CDC) interface for creating a virtual serial port on 32-bit Microchip microcontrollers. This project provides a complete console application that allows command-line interaction through USB, making it an ideal starting point for developing USB-based embedded applications with user interfaces.

This example is built using MPLAB X IDE with Harmony 3 framework and targets the ATSAMD51J19A microcontroller.

## Hardware Information

The example is designed for the ATSAMD51J19A microcontroller with the following features:
- USB 2.0 Full-Speed Device support
- 32-bit ARM Cortex-M4 processor
- Built-in USB transceiver
- GPIO pins for LED control demonstration

### Pin Configuration

| Pin | Description | Usage in Example |
|-----|-------------|------------------|
| PA24 | USB_DM | USB Data Minus |
| PA25 | USB_DP | USB Data Plus |
| PB06 | GPIO Output | LED Control |
| PA16 | GPIO Output | Status LED (toggles automatically) |
| PB07 | GPIO Input  | USB_VBUS_SENSE |

### USB CDC Features
- Virtual COM port functionality
- Line-based command processing
- Command echo and feedback
- Asynchronous read/write operations
- Event-driven architecture

## Project Structure

The project is organized into the following main components:

1. **src/main.cpp**: Main application file with command processing logic
2. **src/app.h/app.cpp**: MPLAB Harmony application framework files
3. **CDC_USB/cdc_usb_platform.h**: CDC USB platform abstraction layer header
4. **CDC_USB/cdc_usb_platform.c**: CDC USB platform implementation
5. **src/config/default/**: MPLAB Harmony configuration files
6. **CDC_Console_USB.X/**: MPLAB X project files

This separation provides a clean architecture where the CDC USB functionality is abstracted into a reusable platform layer, while the application-specific logic remains in the main files.

## CDC USB Platform API Reference

### Initialization

```c
bool cdc_usb_initialize(void);
```

**Description**: Initializes the CDC USB platform, sets up buffers, and starts the initial read transfer if the device is configured.

**Parameters**: None

**Returns**: 
- `true` if initialization was successful and device is configured
- `false` if initialization failed or device is not yet configured

### Writing Data

```c
bool cdc_usb_write(char* data);
```

**Description**: Writes a null-terminated string to the CDC USB interface asynchronously.

**Parameters**:
- `data`: The null-terminated string to transmit

**Returns**: 
- `true` if the write operation was successfully initiated
- `false` if operation failed (device not configured, write in progress, or invalid data)

### Callback Registration

```c
void cdc_usb_return_line_callback_register(void (*callback)(char*));
void cdc_usb_console_ready_callback_register(void (*callback)(void));
```

**Description**: Register callbacks for line completion and console ready events.

**Parameters**:
- `callback`: Function pointer to the callback function (NULL to unregister)

### State Management

```c
cdc_usb_t* get_cdc_usb_handle(void);
```

**Description**: Provides access to the CDC USB state structure for monitoring connection status and transfer states.

**Returns**: Pointer to the global CDC USB state structure

## Data Types and Macros Reference

### Configuration Macros

```c
#define APP_READ_BUFFER_SIZE 512
```
**Description**: Size of the read/write buffers for CDC USB operations. Defines the maximum amount of data that can be processed in a single transfer.

```c
#define CDC_USB_LINE_TERMINATOR '\r'
```
**Description**: Character used to indicate end of command line. When this character is received, the complete command is processed.

```c
#define CDC_USB_RESET_LINE_RESPONSE "\n\rReset line\r\n"
```
**Description**: Message sent to the host when a line reset is triggered by special control characters.

```c
#define CDC_USB_RESET_LINE_CHAR_1 7
#define CDC_USB_RESET_LINE_CHAR_2 8
```
**Description**: ASCII control characters (Bell and Backspace) that trigger command line reset when received.

### Default CDC Line Coding Macros

```c
#define CDC_USB_GET_LINE_CODING_DTERATE 9600
#define CDC_USB_GET_LINE_CODING_PARITY_TYPE 0
#define CDC_USB_GET_LINE_CODING_CHAR_FORMAT 0
#define CDC_USB_GET_LINE_CODING_DATA_BITS 8
```
**Description**: Default values for USB CDC line coding parameters. These define the initial serial port settings presented to the host (baud rate, parity, character format, and data bits).

### Data Structures

#### cdc_usb_t Structure

```c
typedef struct
{
    bool isConfigured;                              // Device configured state
    USB_DEVICE_HANDLE deviceHandle;                 // Device layer handle
    USB_CDC_LINE_CODING setLineCodingData;          // Set Line Coding Data
    USB_CDC_LINE_CODING getLineCodingData;          // Get Line Coding Data
    USB_CDC_CONTROL_LINE_STATE controlLineStateData; // Control Line State
    USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle;  // Read transfer handle
    USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle; // Write transfer handle
    bool isReadComplete;                            // Read operation complete flag
    bool isWriteComplete;                           // Write operation complete flag
    uint16_t breakData;                             // Break data duration
    uint8_t * cdcReadBuffer;                        // Pointer to read buffer
    uint8_t * cdcWriteBuffer;                       // Pointer to write buffer
    uint32_t numBytesRead;                          // Number of bytes read
} cdc_usb_t;
```

**Description**: Main state structure containing all CDC USB operational data.

**Members**:
- `isConfigured`: Indicates whether the USB device is properly configured and ready for communication
- `deviceHandle`: Handle returned by the USB device layer for device operations
- `setLineCodingData`: Line coding configuration received from the host
- `getLineCodingData`: Line coding configuration sent to the host
- `controlLineStateData`: DTR (Data Terminal Ready) and carrier control signals state
- `readTransferHandle`: Handle for tracking asynchronous read operations
- `writeTransferHandle`: Handle for tracking asynchronous write operations
- `isReadComplete`: Flag indicating completion of the current read operation
- `isWriteComplete`: Flag indicating completion of the current write operation
- `breakData`: Duration of break signal received from host (in milliseconds)
- `cdcReadBuffer`: Pointer to the buffer used for receiving data from host
- `cdcWriteBuffer`: Pointer to the buffer used for transmitting data to host
- `numBytesRead`: Number of bytes received in the last read operation

### USB Harmony 3 Related Types

The implementation uses several Harmony 3 USB framework types:

#### USB_DEVICE_HANDLE
```c
typedef uintptr_t USB_DEVICE_HANDLE;
#define USB_DEVICE_HANDLE_INVALID ((USB_DEVICE_HANDLE)(-1))
```
**Description**: Handle type for USB device operations. Used to identify and interact with the USB device instance.

#### USB_DEVICE_CDC_TRANSFER_HANDLE
```c
typedef uintptr_t USB_DEVICE_CDC_TRANSFER_HANDLE;
#define USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID ((USB_DEVICE_CDC_TRANSFER_HANDLE)(-1))
```
**Description**: Handle type for CDC transfer operations. Used to track and manage asynchronous read/write operations.

#### USB_CDC_LINE_CODING
```c
typedef struct
{
    uint32_t dwDTERate;     // Data Terminal Rate (baud rate)
    uint8_t bCharFormat;    // Character format (stop bits)
    uint8_t bParityType;    // Parity type
    uint8_t bDataBits;      // Number of data bits
} USB_CDC_LINE_CODING;
```
**Description**: Structure defining serial communication parameters for the CDC interface.

#### USB_CDC_CONTROL_LINE_STATE
```c
typedef struct
{
    uint8_t dtr : 1;        // Data Terminal Ready
    uint8_t carrier : 1;    // Carrier control
    uint8_t reserved : 6;   // Reserved bits
} USB_CDC_CONTROL_LINE_STATE;
```
**Description**: Structure representing the control line signals state (DTR and carrier).

### Buffer Management

The platform uses statically allocated, cache-aligned buffers:

```c
uint8_t CACHE_ALIGN cdcReadBuffer[APP_READ_BUFFER_SIZE];
uint8_t CACHE_ALIGN cdcWriteBuffer[APP_READ_BUFFER_SIZE];
char commandBuffer[APP_READ_BUFFER_SIZE];
```

**Description**: 
- `cdcReadBuffer`: Cache-aligned buffer for USB read operations
- `cdcWriteBuffer`: Cache-aligned buffer for USB write operations  
- `commandBuffer`: Buffer for accumulating complete command lines

**Note**: The `CACHE_ALIGN` attribute ensures proper memory alignment for DMA operations, which is critical for USB transfers on ARM Cortex-M processors.

### Event Types

The implementation handles various USB device and CDC events:

#### USB_DEVICE_EVENT
- `USB_DEVICE_EVENT_SOF`: Start of Frame event
- `USB_DEVICE_EVENT_RESET`: USB reset event
- `USB_DEVICE_EVENT_CONFIGURED`: Device configuration complete

#### USB_DEVICE_CDC_EVENT
- `USB_DEVICE_CDC_EVENT_GET_LINE_CODING`: Host requests line coding
- `USB_DEVICE_CDC_EVENT_SET_LINE_CODING`: Host sets line coding
- `USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE`: Control line state change
- `USB_DEVICE_CDC_EVENT_SEND_BREAK`: Break signal from host
- `USB_DEVICE_CDC_EVENT_READ_COMPLETE`: Read operation completed
- `USB_DEVICE_CDC_EVENT_WRITE_COMPLETE`: Write operation completed

## Usage Examples

### Example 1: Basic CDC Console Setup

```c
#include "cdc_usb_platform.h"
#include <stdio.h>

void CommandProcessor(char* command);
void ShowWelcomeMessage(void);

int main(void) {
    // Initialize system
    SYS_Initialize(NULL);
    
    // Register callbacks
    cdc_usb_return_line_callback_register(CommandProcessor);
    cdc_usb_console_ready_callback_register(ShowWelcomeMessage);
    
    while(true) {
        SYS_Tasks();
    }
}

void CommandProcessor(char* command) {
    if(strcmp(command, "hello") == 0) {
        cdc_usb_write("Hello World!\r\n");
    } else {
        cdc_usb_write("Unknown command\r\n");
    }
}

void ShowWelcomeMessage(void) {
    cdc_usb_write("Welcome to CDC Console!\r\n");
}
```

### Example 2: LED Control Commands (from main.cpp)

```c
void DecodeCommand(char * command) {
    char txBuffer[256] = {0};
    
    if(strcmp(command, "Turn on led") == 0) {
        GPIO_PB06_Clear();
        sprintf(txBuffer, "\r\nLED is on\r\n");
    } else if(strcmp(command, "Turn off led") == 0) {
        GPIO_PB06_Set();
        sprintf(txBuffer, "\r\nLED is off\r\n");
    } else if(strcmp(command, "Toggle led") == 0) {
        GPIO_PB06_Toggle();
        sprintf(txBuffer, "\r\nLED is toggled\r\n");
    } else {
        sprintf(txBuffer, "\r\nUnknown command\r\n");
    }
    
    cdc_usb_write(txBuffer);
}
```

## USB CDC Communication Protocol

The CDC USB implementation follows the USB Communication Device Class specification:

1. **Device Enumeration**: The device presents itself as a virtual COM port to the host
2. **Line Coding**: Supports standard serial port parameters (baud rate, data bits, parity, stop bits)
3. **Control Signals**: Handles DTR (Data Terminal Ready) and carrier control signals
4. **Data Transfer**: Asynchronous bulk transfers for both read and write operations
5. **Event Handling**: Complete event-driven architecture for robust operation

### Command Processing Flow

1. Host sends characters via USB CDC
2. Characters are buffered until line terminator (`\r`) is received
3. Complete command line triggers registered callback
4. Application processes command and sends response
5. Echo and feedback are automatically handled

## Build and Setup Instructions

### Prerequisites

- MPLAB X IDE v6.00 or later
- XC32 Compiler v4.00 or later
- MPLAB Harmony 3 framework
- Target hardware: ATSAMD51J19A development board

### Building the Project

1. **Open Project**:
   ```bash
   # Navigate to project directory
   cd CDC_Console_USB/CDC_Console_USB.X
   # Open in MPLAB X IDE
   ```

2. **Configure Harmony 3**:
   - Use MPLAB Code Composer (MCC) if modifications are needed
   - Current configuration includes USB Device, CDC, GPIO, and system services

3. **Build Project**:
   - Select "Production" or "Debug" configuration
   - Build using MPLAB X IDE (Ctrl+F11)

4. **Program Device**:
   - Connect target hardware via debugger/programmer
   - Program the device (Ctrl+F5)

### Host Setup

1. **Connect USB Cable**: Connect the device USB port to your computer
2. **Driver Installation**: Windows will automatically install CDC drivers
3. **Open Terminal**: Use any serial terminal application:
   - PuTTY, TeraTerm, Arduino Serial Monitor, etc.
   - Connect to the virtual COM port
   - Settings: Any baud rate (ignored), 8N1

## Available Commands

The example implements the following commands:

| Command | Description | Response |
|---------|-------------|----------|
| `Turn on led` | Turns on the LED connected to PB06 | "led is on" |
| `Turn off led` | Turns off the LED connected to PB06 | "led is off" |
| `Toggle led` | Toggles the LED state | "led is toggled" |
| Any other text | Invalid command | "Unknown command" |

Special characters:
- **Enter (`\r`)**: Executes the current command
- **Ctrl+G or Backspace**: Resets the current command line

## Porting Guide

To port this example to a different Microchip microcontroller:

### Hardware Porting

1. **Update Target Device**: Change the device selection in MPLAB X project settings
2. **Pin Configuration**: Update pin assignments in MCC configuration
3. **Clock Configuration**: Ensure USB clock requirements are met (48MHz for Full-Speed USB)

### Software Porting

1. **Harmony Configuration**: Regenerate code using MCC for the new target
2. **GPIO Updates**: Update GPIO pin definitions in `main.cpp` for LED control
3. **Linker Script**: Update memory configuration for the new device

### Platform Abstraction

The `cdc_usb_platform` module is designed to be portable across Harmony 3 supported devices. The main porting effort involves:

1. Updating Harmony 3 configuration
2. Adjusting GPIO pin assignments
3. Updating project settings for the new target device

## Troubleshooting

### Common Issues

1. **Device Not Recognized**:
   - Check USB cable connection
   - Verify target device is properly programmed
   - Windows: Check Device Manager for driver issues

2. **No Response to Commands**:
   - Ensure terminal is connected to correct COM port
   - Check that commands end with carriage return (`\r`)
   - Verify LED connections match pin assignments

3. **Build Errors**:
   - Ensure all Harmony 3 dependencies are installed
   - Check that XC32 compiler path is correctly configured
   - Verify project configuration matches target device

4. **USB Communication Issues**:
   - Check USB cable for data capability (not just power)
   - Verify clock configuration provides stable 48MHz for USB
   - Review USB event handler for error conditions

### Debugging Tips

1. **LED Status**: PA16 toggles automatically to indicate the program is running
2. **USB Events**: Use debugger to monitor USB event callbacks
3. **Buffer Inspection**: Check `commandBuffer` and USB buffers for data flow
4. **State Monitoring**: Use `get_cdc_usb_handle()` to monitor connection state

## Advanced Features

### Custom Command Implementation

To add new commands:

1. **Extend Command Parser**: Modify `DecodeCommand()` function in `main.cpp`
2. **Add Response Logic**: Implement appropriate responses using `cdc_usb_write()`
3. **Update Menu**: Modify `consoleMenu` string to reflect new commands

### Multiple Callback Registration

The platform supports registering different types of callbacks:

```c
// Register line processing callback
cdc_usb_return_line_callback_register(MyCommandProcessor);

// Register console ready callback
cdc_usb_console_ready_callback_register(MyWelcomeMessage);
```

### Asynchronous Operation

The CDC USB implementation is fully asynchronous:
- Write operations return immediately if successful
- Read operations are handled via callbacks
- No blocking operations in the USB stack

## References

- [USB CDC Class Specification](https://www.usb.org/document-library/class-definitions-communication-devices-12)
- [MPLAB Harmony 3 USB Device Library](https://github.com/Microchip-MPLAB-Harmony/usb)
- [ATSAMD51J19A Datasheet](https://www.microchip.com/en-us/product/ATSAMD51J19A)
- [MPLAB Harmony 3 Documentation](https://github.com/Microchip-MPLAB-Harmony)

## License

This example is part of the FirmwareForge repository and is licensed under the MIT License. See the main repository [LICENSE](../LICENSE) file for details.