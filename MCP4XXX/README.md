# MCP4XXX Library Documentation

## Overview

The MCP4XXX library provides an interface for interacting with the MCP4XXX series of digital potentiometers and rheostats. This library abstracts the I2C communication required to interact with these devices and provides a simple API for controlling wiper positions, reading values, and managing non-volatile memory settings.

## Hardware Information

The MCP4XXX series comprises digital potentiometers and rheostats with the following features:
- Single or dual channel configurations
- 7-bit (129 steps) or 8-bit (257 steps) resolution
- Non-volatile EEPROM memory for wiper settings
- I2C-compatible serial interface (100 kHz, 400 kHz, and 3.4 MHz)
- Single supply operation: 2.7V to 5.5V

### Supported Models

| Model | Channels | Resolution | Type | Steps |
|-------|----------|------------|------|--------|
| MCP4541 | Single | 7-bit | Potentiometer | 129 |
| MCP4542 | Single | 7-bit | Rheostat | 129 |
| MCP4561 | Single | 8-bit | Potentiometer | 257 |
| MCP4562 | Single | 8-bit | Rheostat | 257 |
| MCP4641 | Dual | 7-bit | Potentiometer | 129 |
| MCP4642 | Dual | 7-bit | Rheostat | 129 |
| MCP4661 | Dual | 8-bit | Potentiometer | 257 |
| MCP4662 | Dual | 8-bit | Rheostat | 257 |

### Pin Configuration

| Pin | Description |
|-----|-------------|
| A0, A1, A2 | I2C Address Selection Pins |
| SCL | Serial Clock Input |
| SDA | Serial Data Input/Output |
| VDD | Supply Voltage |
| VSS | Ground |
| P0A, P0W, P0B | Potentiometer 0 Terminals (Terminal A, Wiper, Terminal B) |
| P1A, P1W, P1B | Potentiometer 1 Terminals (Dual devices only) |

## Library Architecture

The library is organized into the following files:

1. **mcp4xxx_api.h**: Main API header file defining the interface functions and data structures.
2. **mcp4xxx_api.c**: Implementation of the API functions.
3. **mcp4xxx_platform.h**: Platform-specific interface declarations.
4. **mcp4xxx_platform.c**: Platform-specific implementation for I2C communication.

This separation allows for easy porting to different hardware platforms by only modifying the platform-specific files while keeping the API consistent.

## API Reference

### Data Types

#### `mcp4xxx_wiper_t`
An enumeration defining the wiper selection for MCP4XXX devices.

| Value | Description |
|-------|-------------|
| `WIPER_0` | Wiper 0 (available on all devices) |
| `WIPER_1` | Wiper 1 (available only on dual channel devices) |

### Constants

#### Command Codes
| Constant | Value | Description |
|----------|-------|-------------|
| `WRITE_CMD` | 0x00 << 2 | Command to write data to a register |
| `INCREMENT_CMD` | 0x01 << 2 | Command to increment wiper value |
| `DECREMENT_CMD` | 0x02 << 2 | Command to decrement wiper value |
| `READ_CMD` | 0x03 << 2 | Command to read data from a register |

#### Register Addresses
| Constant | Value | Description |
|----------|-------|-------------|
| `WIPER_0_ADDRESS` | 0x00 | Volatile wiper 0 register address |
| `WIPER_1_ADDRESS` | 0x01 | Volatile wiper 1 register address (dual devices only) |
| `NV_WIPER_0_ADDRESS` | 0x02 | Non-volatile wiper 0 register address |
| `NV_WIPER_1_ADDRESS` | 0x03 | Non-volatile wiper 1 register address (dual devices only) |
| `TCON_ADDRESS` | 0x04 | Terminal control register address |
| `STATUS_ADDRESS` | 0x05 | Status register address |

### Core Functions

#### `mcp4xxx_check` 
``` c
bool mcp4xxx_check(uint8_t device_address)
```
Checks if an MCP4XXX device is present and functioning.

**Parameters:**
- `device_address`: The I2C device address for the target device

**Returns:**
- `true` if the device responds correctly
- `false` otherwise

#### `mcpxxx_write`
```c
bool mcpxxx_write(uint8_t device_address, uint8_t reg_address, uint16_t data)
```
Writes data to a specified register on an MCP4XXX device.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `reg_address`: Register address to write
- `data`: 16-bit data to write to the register (only 9 bits are used)

**Returns:**
- `true` if write was successful
- `false` otherwise

#### `mcpxxx_read`
```c
uint16_t mcpxxx_read(uint8_t device_address, uint8_t reg_address)
```
Reads data from a specified register on an MCP4XXX device.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `reg_address`: Register address to read from

**Returns:**
- 16-bit data read from the register

### Wiper Control Functions

#### `mcp4xxx_set_wiper`
```c
bool mcp4xxx_set_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value)
```
Sets the wiper position to a specific value.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to set (WIPER_0 or WIPER_1 for dual devices)
- `value`: The wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)

**Returns:**
- `true` if the wiper was set successfully
- `false` otherwise

#### `mcp4xxx_get_wiper`
```c
uint16_t mcp4xxx_get_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
```
Gets the current wiper position.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to read (WIPER_0 or WIPER_1 for dual devices)

**Returns:**
- Current wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)

#### `mcp4xxx_increment_wiper`
```c
bool mcp4xxx_increment_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
```
Increments the wiper position by one step.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to increment (WIPER_0 or WIPER_1 for dual devices)

**Returns:**
- `true` if the operation was successful
- `false` otherwise

**Note:** If the wiper is already at the maximum position, it typically remains unchanged.

#### `mcp4xxx_decrement_wiper`
```c
bool mcp4xxx_decrement_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
```
Decrements the wiper position by one step.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to decrement (WIPER_0 or WIPER_1 for dual devices)

**Returns:**
- `true` if the operation was successful
- `false` otherwise

**Note:** If the wiper is already at the minimum position (0), it typically remains unchanged.

### Non-Volatile Memory Functions

#### `mcp4xxx_set_nv_wiper`
```c
bool mcp4xxx_set_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value)
```
Sets the non-volatile wiper position to a specific value.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to set (WIPER_0 or WIPER_1 for dual devices)
- `value`: The wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)

**Returns:**
- `true` if the operation was successful
- `false` otherwise

**Note:** This function writes to the non-volatile memory in the device. The MCP4XXX devices have a limited number of write cycles for non-volatile memory, so use this function sparingly.

#### `mcp4xxx_get_nv_wiper`
```c
uint16_t mcp4xxx_get_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
```
Gets the non-volatile wiper position.

**Parameters:**
- `device_address`: The I2C device address for the target device
- `wiper`: Which wiper to read (WIPER_0 or WIPER_1 for dual devices)

**Returns:**
- Non-volatile wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)

## Usage Examples

### Example 1: Basic Potentiometer Control

```c
#include "mcp4xxx_api.h"
#include <stdio.h>

#define MCP4561_ADDRESS 0x2C  // Example I2C address (A0=0, A1=1, A2=1)

int main(void) {
    // Check if the device is responding
    if (!mcp4xxx_check(MCP4561_ADDRESS)) {
        printf("Error: MCP4561 not detected!\n");
        return -1;
    }
    
    // Set wiper 0 to mid-scale (128 for 257-step device)
    mcp4xxx_set_wiper(MCP4561_ADDRESS, WIPER_0, 128);
    
    // Read back the wiper position
    uint16_t wiper_value = mcp4xxx_get_wiper(MCP4561_ADDRESS, WIPER_0);
    printf("Wiper 0 position: %u\n", wiper_value);
    
    return 0;
}
```

### Example 2: Controlling a Dual-Channel Device

```c
#include "mcp4xxx_api.h"
#include <stdio.h>

#define MCP4662_ADDRESS 0x2E  // Example I2C address

int main(void) {
    // Set both wipers to different positions
    mcp4xxx_set_wiper(MCP4662_ADDRESS, WIPER_0, 200);  // Channel 0
    mcp4xxx_set_wiper(MCP4662_ADDRESS, WIPER_1, 50);   // Channel 1
    
    // Increment wiper 0 five times
    for (int i = 0; i < 5; i++) {
        mcp4xxx_increment_wiper(MCP4662_ADDRESS, WIPER_0);
    }
    
    // Store wiper settings to non-volatile memory
    mcp4xxx_set_nv_wiper(MCP4662_ADDRESS, WIPER_0, 
                          mcp4xxx_get_wiper(MCP4662_ADDRESS, WIPER_0));
    mcp4xxx_set_nv_wiper(MCP4662_ADDRESS, WIPER_1, 
                          mcp4xxx_get_wiper(MCP4662_ADDRESS, WIPER_1));
    
    return 0;
}
```

## I2C Communication Protocol

The MCP4XXX uses a standard I2C protocol with the following sequence:

1. Master sends START condition
2. Master sends 7-bit device address followed by write bit (0)
3. Device acknowledges
4. Master sends command byte (register address + command)
5. For writes:
   - Master sends data byte(s)
   - Device acknowledges each byte
6. For reads:
   - Master sends repeated START
   - Master sends device address with read bit (1)
   - Device sends data byte(s)
   - Master acknowledges each byte except the last one
7. Master sends STOP condition

## Integration Guide

To port this library to a different platform:

1. Modify `mcp4xxx_platform.c` to implement the I2C communication functions for your specific hardware:
   - `mcp4xxx_i2c_write()`
   - `mcp4xxx_i2c_read()`
   - `mcp4xxx_i2c_write_byte()`
2. Ensure your I2C configuration matches the requirements of the MCP4XXX (100 kHz or 400 kHz I2C clock).

### Example Platform Implementation for Microchip SAMD51 with [MPLABX MCC Harmony](https://github.com/Microchip-MPLAB-Harmony)

This is an example implementation using Microchip's SAMD51 microcontroller with the SERCOM4 peripheral configured as an I2C master through MPLAB Harmony:

```c

#include "mcp4xxx_platform.h"
#include <stdbool.h>

// Include the I2C driver header from MPLAB Harmony
#include "peripheral/sercom/i2c_master/plib_sercom4_i2c_master.h"

bool mcp4xxx_i2c_write(uint8_t device_address, uint16_t data)
{
    uint8_t wrData[2] = {0};
    bool busStatus = false;
    
    wrData[0] = data >> 8;            // MSB
    wrData[1] = data & 0xFF;          // LSB

    busStatus = SERCOM4_I2C_Write(device_address, wrData, 2); // Write data to I2C bus
    if(!busStatus){
        return false; // Failure
    }
    while (SERCOM4_I2C_IsBusy());     // Wait for I2C bus to be free
    return true;
}

uint16_t mcp4xxx_i2c_read(uint8_t device_address, uint8_t read_command)
{
    uint8_t rData[2] = {0};
    bool busStatus = false;
    
    // Write the command byte
    busStatus = SERCOM4_I2C_Write(device_address, &read_command, 1);
    if(!busStatus){
        return 0xFFFF; // Failure
    }
    while (SERCOM4_I2C_IsBusy());     // Wait for I2C bus to be free

    // Read 2 bytes of response
    busStatus = SERCOM4_I2C_Read(device_address, rData, 2);
    if(!busStatus){
        return 0xFFFF; // Failure
    }
    while (SERCOM4_I2C_IsBusy());     // Wait for I2C bus to be free

    return rData[0] << 8 | rData[1];  // Return combined 16-bit value
}

bool mcp4xxx_i2c_write_byte(uint8_t device_address, uint8_t data){
    bool busStatus;
    busStatus = SERCOM4_I2C_Write(device_address, &data, 1);
    if(!busStatus){
        return false; // Failure
    }
    while (SERCOM4_I2C_IsBusy());     // Wait for I2C bus to be free
    return true;
}
```

#### Adapting to Other Platforms

To adapt this code for different microcontrollers or platforms:

1. **For STM32 platforms** using HAL:
   - Replace the SERCOM4_I2C_ functions with HAL_I2C_ equivalents
   - Use the correct I2C handle for your hardware configuration

2. **For Arduino**:
   - Use Wire library functions (Wire.beginTransmission, Wire.write, Wire.endTransmission, etc.)
   - Remember that Arduino uses 7-bit addressing, so shift the device address right by 1

3. **For ESP32**:
   - Use the esp-idf I2C driver functions or Arduino Wire library if using Arduino framework

4. **For other platforms**:
   - Implement these functions using the specific I2C driver functions available for your platform
   - Maintain the same function signatures to keep compatibility with the MCP4XXX API

## Troubleshooting

### Common Issues:

1. **Device not responding**: Check I2C address settings and make sure pull-up resistors are installed on SCL/SDA lines.
2. **Unexpected wiper values**: Verify the device type and ensure you're using the correct range (0-128 or 0-256).
3. **Settings not saved after power cycle**: Make sure to use the non-volatile memory functions to store wiper positions.
4. **I2C communication errors**: Check bus speed and ensure proper signal integrity with appropriate pull-up resistors.

## References

- [MCP4XXX Family Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22107B.pdf)
- [I2C Bus Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
