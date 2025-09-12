# ADS8866 ADC Library Documentation

## Overview

This library provides a simple interface for the Texas Instruments ADS8866 16-bit analog-to-digital converter (ADC). The ADS8866 is a high-precision, low-power, single-channel ADC with an SPI interface, ideal for applications requiring high resolution and accuracy.

## Hardware Information

- Complete 16-bit resolution ADC interface (0-65535)
- Simple API with minimal function calls
- Automatic conversion from digital values to voltage
- Configurable reference voltage
- Platform-independent architecture with hardware abstraction layer
- Low overhead implementation for embedded systems

### Technical Specifications

| Parameter | Value |
|-----------|-------|
| Resolution | 16-bit |
| Interface | 16 MHz SPI |
| Sampling Rate | 100 kSPS |
| Analog Supply Range | 2.7V to 3.6V |
| Digital Supply Range | 1.65V to 5.5V |
| Reference Voltage | 2.5V to 5.0V |
| Input Range | 0V to VREF |

### Pin Configuration

The ADS8866 requires the following connections to your microcontroller:

```
┌───────────┐                  ┌──────────────┐
│           │                  │              │
│       SCLK├──────────────────│SCLK          │
│           │                  │              │
│        SDO├──────────────────│MISO          │
│  ADS8866  │                  │      MCU     │
│           │                  │              │
│     CONVST├──────────────────│CS            │
│           │                  │              │
└───────────┘                  └──────────────┘
```

## Library Architecture

The library is organized into the following files:

1. **ads8866_api.h**: Main API header file defining the interface functions and data structures.
2. **ads8866_api.c**: Implementation of the API functions.
3. **ads8866_platform.h**: Platform-specific interface declarations.
4. **ads8866_platform.c**: Platform-specific implementation for SPI communication.

This separation allows for easy porting to different hardware platforms by only modifying the platform-specific files while keeping the API consistent.

## API Reference

### Reading Conversion

```c
ads8866_data_t ads8866_read(void);
```
**Description**: Reads a conversion from the ADC via SPI interface
and calculates the corresponding voltage based on the reference voltage.

**Parameters**: None

**Returns**: ads8866_data_t Structure containing both digital and analog values

### Setting Reference Voltage

```c
bool ads8866_set_vref(float vref);
```
**Description**: Set the reference voltage for ADC calculations.

The reference voltage is used to convert the digital readings to voltage values. Valid range is 2.5V to 5.0V for the ADS8866.

**Parameters**: vref Reference voltage in volts

**Returns**: bool true if successful, false if vref is out of valid range

### Getting Reference Voltage

```c
float ads8866_get_vref(void);
```

**Description**: Get the current reference voltage setting

**Parameters**: None

**Returns**: float - reference voltage in volts

### Data Structures

```c
typedef struct {
    uint16_t digital_value;  /**< Digital value read from ADC (0-65535) */
    float voltage;           /**< Voltage value calculated from digital value (in volts) */
} ads8866_data_t; 
```

**Description**: This structure holds both the raw digital value read from the ADC and the corresponding calculated voltage.

## Usage Examples

### Basic Reading

```c
#include "ads8866_api.h"

void read_adc_example(void) {
    // Read ADC value with single function call
    ads8866_data_t result = ads8866_read();
    
    // Access raw digital value (0-65535)
    uint16_t digital = result.digital_value;
    
    // Access calculated voltage value
    float voltage = result.voltage;
    
    // Use values as needed...
}
```

### Setting Reference Voltage

```c
// Configure reference voltage (must match hardware setup)
if (ads8866_set_vref(5.0f)) {
    // Success - valid reference voltage
} else {
    // Error handling - invalid reference voltage
}

// Get current reference voltage setting
float current_vref = ads8866_get_vref();
```
## Integration Guide

To port this library to a different platform:

1. Modify `ads8866_platform.c` to implement the `ads8866_platform_spi_read()` function for your specific hardware.
2. Ensure your SPI configuration matches the requirements of the ADS8866 (Mode 0, MSB first).
3. Include the appropriate headers for your platform's SPI driver.

### Example Platform Implementation for ATSAMD21J18 and [MPLABX MCC Harmony](https://github.com/Microchip-MPLAB-Harmony):

```c
#include "ads8866_platform.h"
#include <stdint.h>
#include "peripheral/sercom/spi_master/plib_sercom0_spi_master.h"

uint16_t ads8866_platform_spi_read(void){
    uint8_t rxBuffer[2] = {0};
    uint16_t receivedData = 0;
    SERCOM0_SPI_Read(rxBuffer, 2);
    receivedData = (rxBuffer[0] << 8) | rxBuffer[1]; // Combine the two bytes into a single 16-bit value
    return receivedData; // Return the received data
}
```

## Troubleshooting

### Common Issues

1. **Always Reading Zero or Incorrect Values**

- **Check SPI wiring:** Ensure SCLK, SDO, and CONVST are correctly connected between MCU and ADS8866.
- **SPI configuration:** Confirm SPI is set to Mode 0 (CPOL=0, CPHA=0), MSB first, and correct clock speed (≤16 MHz).
- **Platform implementation:** Verify that `ads8866_platform_spi_read()` correctly reads 2 bytes and combines them as shown in the example.

2. **Voltage Calculation Seems Wrong**

- **Reference voltage setting:** Make sure `ads8866_set_vref()` is called with the correct value matching your hardware setup.
- **Valid range:** Only values between 2.5V and 5.0V are accepted. If out of range, the function returns `false` and the voltage calculation will use the previous value.

3. **`ads8866_set_vref()` Always Returns False**

- **Input value:** Ensure the value passed is within 2.5V to 5.0V.
- **Floating point precision:** Use a float literal (e.g., `3.3f`) to avoid type mismatch.

4. ** Compilation Errors**

- **Missing platform SPI driver:** Make sure your project includes the correct SPI driver headers (e.g., `plib_sercom0_spi_master.h` for ATSAMD21J18).
- **Header inclusion:** All API and platform headers must be included as shown in the usage examples.

5. **No Response from ADC**

- **Power supply:** Confirm analog and digital supply voltages are within the specified ranges.
- **Chip select (CS/CONVST):** Ensure the MCU is properly toggling the CONVST pin to initiate conversions.

## References

- [ADS8866 Datasheet](https://www.ti.com/lit/ds/symlink/ads8866.pdf)

