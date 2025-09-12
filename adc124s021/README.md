# ADC124S021 Library Documentation

## Overview

The ADC124S021 library provides an interface for interacting with the ADC124S021 4-channel, 12-bit Analog-to-Digital Converter. This library abstracts the SPI communication required to interact with the ADC and provides a simple API for reading analog values from the device.

## Hardware Information

The ADC124S021 is a 4-channel, 12-bit successive approximation ADC with the following features:
- 12-bit resolution
- 4 analog input channels
- SPI-compatible serial interface
- 50 ksps to 200 ksps sampling rate
- Single supply operation: 2.7V to 5.25V

### Pin Configuration

| Pin | Description |
|-----|-------------|
| CS  | Chip Select (Active Low) |
| SCLK | Serial Clock Input |
| DIN | Data Input (for channel selection) |
| DOUT | Data Output |
| VA | Analog Supply Voltage |
| GND | Ground |
| IN0 - IN3 | Analog Inputs |

## Library Architecture

The library is organized into the following files:

1. **adc124s021_api.h**: Main API header file defining the interface functions and data structures.
2. **adc124s021_api.c**: Implementation of the API functions.
3. **adc124s021_platform.h**: Platform-specific interface declarations.
4. **adc124s021_platform.c**: Platform-specific implementation for SPI communication.

This separation allows for easy porting to different hardware platforms by only modifying the platform-specific files while keeping the API consistent.

## API Reference

### Initialization

```c
void adc124s021_init(void);
```

**Description**: Initializes the ADC124S021 by setting up the SPI interface and performing a dummy read. Must be called before using any other functions from this library.

**Parameters**: None

**Returns**: None

### Reading a Single Channel

```c
uint16_t adc124s021_read_channel(uint8_t channel);
```

**Description**: Reads the ADC value from a specified channel.

**Parameters**:
- `channel`: The ADC channel to read (0-3)

**Returns**: 
- The 12-bit ADC value from the specified channel (0-4095)
- 0xFFFF if the channel is invalid

### Reading All Channels

```c
adc124s021_data_t adc124s021_read_all_channels(void);
```

**Description**: Reads the ADC values from all 4 channels (0-3) in sequence.

**Parameters**: None

**Returns**: An `adc124s021_data_t` struct containing:
- ADC values for all 4 channels in the `channel` array
- Voltage values for all 4 channels in the `voltage` array

### Data Structures

```c
typedef struct {
    uint16_t channel[4];    // ADC values for channels 0-3
    float voltage[4];       // Voltage values for channels 0-3
} adc124s021_data_t;
```

**Description**: Structure containing the ADC values and corresponding voltage values for all 4 channels.

## Usage Examples

### Example 1: Initializing the ADC and Reading a Single Channel

```c
#include "adc124s021_api.h"
#include <stdio.h>

int main(void) {
    // Initialize the ADC
    adc124s021_init();
    
    // Read a single channel (channel 2)
    uint16_t adc_value = adc124s021_read_channel(2);
    
    // Convert to voltage (assuming 3.3V reference)
    float voltage = adc_value * (3.3f / 4095.0f);
    
    printf("ADC Channel 2: %u (%.2fV)\n", adc_value, voltage);
    
    return 0;
}
```

### Example 2: Reading All Channels

```c
#include "adc124s021_api.h"
#include <stdio.h>

int main(void) {
    // Initialize the ADC
    adc124s021_init();
    
    // Read all channels
    adc124s021_data_t adc_data = adc124s021_read_all_channels();
    
    // Print the results
    for (int i = 0; i < 4; i++) {
        printf("Channel %d: %u (%.2fV)\n", i, adc_data.channel[i], adc_data.voltage[i]);
    }
    
    return 0;
}
```

## SPI Communication Protocol

The ADC124S021 uses a simple SPI protocol:

1. To initiate a conversion, the CS line is brought low.
2. A 16-bit word is transmitted, with the first 3 bits (14-12) indicating the next channel to be converted:
   - 000: Channel 0
   - 001: Channel 1
   - 010: Channel 2
   - 011: Channel 3
3. The next 12 bits returned by the ADC contain the result of the previous conversion.
4. After sending all 16 bits, CS may be brought high or kept low for the next conversion.

## Integration Guide

To port this library to a different platform:

1. Modify `adc124s021_platform.c` to implement the `adc124s021_platform_spi_transfer()` function for your specific hardware.
2. Ensure your SPI configuration matches the requirements of the ADC124S021 (Mode 0, MSB first).
3. Include the appropriate headers for your platform's SPI driver.

### Example Platform Implementation for ATSAMD21J18 and [MPLABX MCC Harmony](https://github.com/Microchip-MPLAB-Harmony):

```c
#include "adc124s021_platform.h"
#include "peripheral/sercom/spi_master/plib_sercom0_spi_master.h"

uint16_t adc124s021_platform_spi_transfer(uint16_t data) {
    uint16_t receivedData = 0;
    uint8_t txData[2] = { (data >> 8) & 0xFF, data & 0xFF }; 
    uint8_t rxData[2] = { 0, 0 };
    SERCOM0_SPI_WriteRead(txData, 2, rxData, 2);
    receivedData = (rxData[0] << 8) | rxData[1]; // Combine the two bytes into a single 16-bit value
    return receivedData; // Return the received data
}
```

## Troubleshooting

### Common Issues:

1. **Reading all zeros**: Check the SPI connections and ensure CS is correctly toggled.
2. **Reading 0xFFFF**: You're either using an invalid channel or the SPI communication is failing.
3. **Unstable readings**: Check for proper grounding and decoupling capacitors on the ADC power supply.
4. **Cross-channel interference**: Ensure proper isolation between analog inputs.

## References

- [ADC124S021 Datasheet](https://www.ti.com/product/ADC124S021)
