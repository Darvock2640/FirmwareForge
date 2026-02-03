# TMP1075 Library Documentation

## Overview

The TMP1075 library provides an interface for interacting with the TMP1075 temperature sensor over I2C. This library abstracts the I2C communication required to interact with the device and provides a simple API for reading temperature, configuring alerts, and managing power modes.

## Hardware Information

The TMP1075 is a digital temperature sensor with the following features:
- 12-bit resolution (0.0625°C)
- Temperature range: -55°C to +150°C
- Accuracy: ±1°C (max) from -40°C to +110°C
- I2C/SMBus compatible interface
- Programmable over-temperature alert
- One-shot conversion mode for power saving
- Shutdown mode

### Pin Configuration

| Pin | Description |
|-----|-------------|
| SDA | Serial Data Input/Output |
| SCL | Serial Clock Input |
| ALERT | Over-temperature Alert Output |
| V+  | Supply Voltage (1.7V to 5.5V) |
| GND | Ground |
| A0, A1, A2 | Address Selection Pins (depending on package) |

## Library Architecture

The library is organized into the following files:

1. **tmp1075_api.h**: Main API header file defining the interface functions, enums, and macros.
2. **tmp1075_api.c**: Implementation of the API functions.
3. **tmp1075_platform.h**: Platform-specific interface declarations.
4. **tmp1075_platform.c**: Platform-specific implementation for I2C communication.

This separation allows for easy porting to different hardware platforms by only modifying the platform-specific files while keeping the API consistent.

## API Reference

### Data Types

#### `tmp1075_conversion_rate`
An enumeration defining the conversion rate settings.

| Value | Description |
|-------|-------------|
| `TMP1075_CONVERSION_RATE_27_5_MS` | 27.5 ms conversion rate |
| `TMP1075_CONVERSION_RATE_55_MS` | 55 ms conversion rate |
| `TMP1075_CONVERSION_RATE_110_MS` | 110 ms conversion rate |
| `TMP1075_CONVERSION_RATE_220_MS` | 220 ms conversion rate |

#### `tmp1075_consecutive_fault`
An enumeration defining the fault queue settings.

| Value | Description |
|-------|-------------|
| `TMP1075_FAULT_QUEUE_1` | 1 Consecutive fault measurement |
| `TMP1075_FAULT_QUEUE_2` | 2 Consecutive fault measurements |
| `TMP1075_FAULT_QUEUE_3` | 3 Consecutive fault measurements |
| `TMP1075_FAULT_QUEUE_4` | 4 Consecutive fault measurements |

#### `tmp1075_alert_polarity`
An enumeration defining the alert pin polarity.

| Value | Description |
|-------|-------------|
| `TMP1075_ALERT_POLARITY_ACTIVE_LOW` | Active low ALERT pin |
| `TMP1075_ALERT_POLARITY_ACTIVE_HIGH` | Active high ALERT pin |

#### `tmp1075_alert_function`
An enumeration defining the alert function mode.

| Value | Description |
|-------|-------------|
| `TMP1075_ALERT_FUNCTION_COMPARATOR` | Comparator mode |
| `TMP1075_ALERT_FUNCTION_INTERRUPT` | Interrupt mode |

#### `tmp1075_power_mode`
An enumeration defining the power mode.

| Value | Description |
|-------|-------------|
| `TMP1075_POWER_MODE_CONTINUOUS` | Continuous conversion mode |
| `TMP1075_POWER_MODE_SHUTDOWN` | Shutdown mode |

### Core Functions

#### `tmp1075_read_temperature`
```c
float tmp1075_read_temperature(uint8_t device_address);
```
Reads the current temperature from the device.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device

**Returns:**
- The current temperature in Celsius.

#### `tmp1075_id_read`
```c
uint16_t tmp1075_id_read(uint8_t device_address);
```
Reads the device ID of the TMP1075.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device

**Returns:**
- The 16-bit device ID.

#### `tmp1075_set_power_mode`
```c
bool tmp1075_set_power_mode(uint8_t device_address, tmp1075_power_mode mode);
```
Sets the power mode of the device (Continuous or Shutdown).

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `mode`: The desired power mode

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_one_shot_conversion`
```c
bool tmp1075_one_shot_conversion(uint8_t device_address);
```
Triggers a single temperature conversion when the device is in shutdown mode.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_conversion_rate`
```c
bool tmp1075_set_conversion_rate(uint8_t device_address, tmp1075_conversion_rate rate);
```
Sets the rate at which temperature conversions are performed in continuous mode.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `rate`: The desired conversion rate

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_fault_queue`
```c
bool tmp1075_set_fault_queue(uint8_t device_address, tmp1075_consecutive_fault faults);
```
Sets the number of consecutive faults required to trigger an alert.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `faults`: The number of consecutive faults

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_alert_polarity`
```c
bool tmp1075_set_alert_polarity(uint8_t device_address, tmp1075_alert_polarity polarity);
```
Sets the polarity of the ALERT pin.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `polarity`: The desired polarity

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_alert_function`
```c
bool tmp1075_set_alert_function(uint8_t device_address, tmp1075_alert_function function);
```
Sets the mode of the ALERT pin (Comparator or Interrupt).

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `function`: The desired alert function

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_temperature_low_limit`
```c
bool tmp1075_set_temperature_low_limit(uint8_t device_address, float temp_low);
```
Sets the low temperature limit for the ALERT function.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `temp_low`: The temperature limit in Celsius

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_set_temperature_high_limit`
```c
bool tmp1075_set_temperature_high_limit(uint8_t device_address, float temp_high);
```
Sets the high temperature limit for the ALERT function.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device
- `temp_high`: The temperature limit in Celsius

**Returns:**
- `true` if successful, `false` otherwise.

#### `tmp1075_read_temperature_low_limit`
```c
float tmp1075_read_temperature_low_limit(uint8_t device_address);
```
Reads the low temperature limit.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device

**Returns:**
- The low temperature limit in Celsius.

#### `tmp1075_read_temperature_high_limit`
```c
float tmp1075_read_temperature_high_limit(uint8_t device_address);
```
Reads the high temperature limit.

**Parameters:**
- `device_address`: The 7-bit I2C address of the target device

**Returns:**
- The high temperature limit in Celsius.

## Usage Examples

### Example 1: Basic Temperature Reading and Configuration

```c
#include "tmp1075_api.h"
#include <stdio.h>

#define TMP1075_ADDR 0x48

int main(void) {
    // Initialize platform I2C (implementation specific)
    
    // Check device ID
    uint16_t id = tmp1075_id_read(TMP1075_ADDR);
    printf("Device ID: 0x%04X\n", id);
    
    // Configure device
    tmp1075_set_conversion_rate(TMP1075_ADDR, TMP1075_CONVERSION_RATE_110_MS);
    tmp1075_set_alert_polarity(TMP1075_ADDR, TMP1075_ALERT_POLARITY_ACTIVE_LOW);
    
    // Set limits
    tmp1075_set_temperature_high_limit(TMP1075_ADDR, 85.0f);
    tmp1075_set_temperature_low_limit(TMP1075_ADDR, 10.0f);
    
    while(1) {
        float temp = tmp1075_read_temperature(TMP1075_ADDR);
        printf("Temperature: %.2f C\n", temp);
        
        // Add delay
    }
    return 0;
}
```

## Integration Guide

To port this library to a different platform:

1. Modify `tmp1075_platform.c` to implement the I2C communication functions for your specific hardware:
   - `tmp1075_i2c_write()`
   - `tmp1075_i2c_read()`
   - `tmp1075_i2c_write_byte()`
2. Ensure your I2C configuration matches the requirements of the TMP1075.

### Example Platform Implementation for Microchip SAMD51 with [MPLABX MCC Harmony](https://github.com/Microchip-MPLAB-Harmony)

This is an example implementation using Microchip's SAMD51 microcontroller with the SERCOM4 peripheral configured as an I2C master through MPLAB Harmony:

```c
#include "tmp1075_platform.h"
#include "peripheral/sercom/i2c_master/plib_sercom5_i2c_master.h"

static uint8_t RxBuffer[5] ={0};
static uint8_t TxBuffer[5] ={0};

bool tmp1075_i2c_write(uint8_t device_address, uint32_t data){
    TxBuffer[0] = (data >> 16) & 0xFF; // Register address
    TxBuffer[1] = (data >> 8) & 0xFF;
    TxBuffer[2] = data & 0xFF;
    
    while(SERCOM5_I2C_IsBusy()){
        if(SERCOM5_I2C_ErrorGet() != SERCOM_I2C_ERROR_NONE){
            return false;
        }
    }
    
    return SERCOM5_I2C_Write(device_address, TxBuffer, 3);
}

bool tmp1075_i2c_write_byte(uint8_t device_address, uint8_t data){
    TxBuffer[0] = data;
    
    while(SERCOM5_I2C_IsBusy()){
        if(SERCOM5_I2C_ErrorGet() != SERCOM_I2C_ERROR_NONE){
            return false;
        }
    }
    
    return SERCOM5_I2C_Write(device_address, TxBuffer, 1);
}

uint16_t tmp1075_i2c_read(uint8_t device_address){
    while(SERCOM5_I2C_IsBusy()){
        if(SERCOM5_I2C_ErrorGet() != SERCOM_I2C_ERROR_NONE){
            return 0xFFFF;
        }
    }
    SERCOM5_I2C_Read(device_address, RxBuffer, 2);
    while(SERCOM5_I2C_IsBusy()){
        if(SERCOM5_I2C_ErrorGet() != SERCOM_I2C_ERROR_NONE){
            return 0xFFFF;
        }
    }
    return ( (RxBuffer[0] << 8) | RxBuffer[1] );
}
```

## Troubleshooting

### Common Issues:

1. **Device not responding**: Check I2C address settings and make sure pull-up resistors are installed on SCL/SDA lines.
2. **Incorrect temperature readings**: Ensure the sensor is not placed near heat-generating components unless intended.
3. **Alert pin not triggering**: Verify the polarity and mode settings, and ensure the limits are set correctly.

## References

- [TMP1075 Datasheet](https://www.ti.com/lit/ds/symlink/tmp1075.pdf)
- [I2C Bus Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
