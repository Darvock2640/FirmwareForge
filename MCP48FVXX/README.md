# MCP48FVXX DAC Library Documentation

## Overview

The MCP48FVXX library provides an API for interfacing with MCP48FVXX series Digital-to-Analog Converters (DACs). These are Single- and Dual-channel 8-bit, 10-bit, and 12-bit buffered voltage output Digital-to-Analog Converters (DAC) with volatile memory and an SPI serial interface.

## Hardware Information

### Supported MCP48FVXX Chips

**Single Channel DACs:**
- **MCP48FVB01** - 8-bit, Single Channel, Buffered Voltage Output
- **MCP48FVB11** - 10-bit, Single Channel, Buffered Voltage Output  
- **MCP48FVB21** - 12-bit, Single Channel, Buffered Voltage Output

**Dual Channel DACs:**
- **MCP48FVB02** - 8-bit, Dual Channel, Buffered Voltage Output
- **MCP48FVB12** - 10-bit, Dual Channel, Buffered Voltage Output
- **MCP48FVB22** - 12-bit, Dual Channel, Buffered Voltage Output

### Key Features

- Resolution: 8-bit, 10-bit, or 12-bit depending on model
- Single or Dual channel output
- SPI-compatible serial interface (Up to 20 MHz writes and 10 MHz reads)
- Individual channel power down control
- Operating voltage: 2.7V to 5.5V

### Pin Configuration

| Pin | Name    | Description |
|-----|---------|-------------|
| 1   | VDD     | Positive Supply Voltage (2.7V to 5.5V) |
| 2   | CS      | Chip Select (Active Low) |
| 3   | VREF    | External Voltage Reference Input |
| 4   | VOUT0   | DAC A Analog Output |
| 5   | VOUT1   | DAC B Analog Output (Dual channel only)   |
| 6   | LAT/HVC | DAC Register Latch/High-Voltage |
| 7   | VSS     | Ground |
| 8   | SDO     | Serial Data Output (for readback) |
| 9   | SCK     | Serial Clock Input (up to 20 MHz) |
| 10  | SDI     | Serial Data Input |

### SPI Interface Specifications

- **SPI Mode:** Mode 0 (CPOL=0, CPHA=0) or Mode 3 (CPOL=1, CPHA=1)
- **Data Format:** MSB first, 24-bit commands
- **Clock Frequency:** Up to 20 MHz
- **Chip Select:** Active low, must remain low during entire 24-bit transfer

## Library Architecture

The library is organized into the following files:

1. **mcp48fvxx_api.h**: Main API header file defining the interface functions and data structures.
2. **mcp48fvxx_api.c**: Implementation of the API functions.
3. **mcp48fvxx_platform.h**: Platform-specific interface declarations.
4. **mcp48fvxx_platform.c**: Platform-specific implementation for SPI communication.

This separation allows for easy porting to different hardware platforms by only modifying the platform-specific files while keeping the API consistent.

## API Reference

### Constants

#### Channel Selection

```c
#define MCP48FVXX_CHANNEL_A 0x00    // Select channel A
#define MCP48FVXX_CHANNEL_B 0x01    // Select channel B
```

#### Register Addresses

```c
#define MCP48FVXX_CHANNEL_A_ADDRESS 0x00
#define MCP48FVXX_CHANNEL_B_ADDRESS (0x01 << 19)
#define MCP48FVXX_ON_OFF_REG (0x09 << 19)    // Power control register
```

#### Command Types

```c
#define MCP48FVXX_WRITE 0b00
#define MCP48FVXX_READ (0b11 << 17)
```

#### Channel Power States

```c
#define MCP48FVXX_CHANNEL_ON 0x00
#define MCP48FVXX_CHANNEL_OFF 0x01
```

### Functions

#### Setting DAC Output

```c
bool mcp48fvxx_set_output(uint8_t channel, uint16_t value);
```
**Description**: Sets the output value for the specified DAC channel.

**Parameters**:
- `channel`: MCP48FVXX_CHANNEL_A or MCP48FVXX_CHANNEL_B
- `value`: 12-bit output value (0-4095)

**Returns**:
- Returns: `true` if successful, `false` if an error occurred

#### Channel Power Control

```c
bool mcp48fvxx_channel_on_off(uint8_t channel, bool on_off);
```
**Description**: Turns a DAC channel on or off.

**Parameters**:
- `channel`: MCP48FVXX_CHANNEL_A or MCP48FVXX_CHANNEL_B
- `on_off`: `true` to turn the channel on, `false` to turn it off

**Returns**:
- Returns: `true` if successful, `false` if an error occurred

### Platform-Specific Functions

These functions need to be implemented for your specific platform:

#### SPI Transfer

```c
uint32_t mcp48fvxx_spi_transfer(uint32_t command_24bit);
```

**Description**:
Performs a 24-bit SPI transfer with the DAC.

**Parameters**:
- `command_24bit`: 24-bit command to send to the DAC

**Returns**:
- 24-bit response from the DAC

#### Error Handler

```c
void mcp48fvxx_error_handler(char *message);
```

**Description**:
Handles error messages from the DAC library.

**Parameters**:
- `message`: Error message string

**Returns**: None

## Usage Examples

### Example 1: Setting DAC Output

```c
#include "mcp48fvxx_api.h"

int main(void) {
    // Set Channel A to mid-scale (2048)
    mcp48fvxx_set_output(MCP48FVXX_CHANNEL_A, 2048);

    // Set Channel B to full-scale (4095)
    mcp48fvxx_set_output(MCP48FVXX_CHANNEL_B, 4095);

    return 0;
}
```

### Example 2: Channel Power Control

```c
#include "mcp48fvxx_api.h"

int main(void) {
    // Turn on Channel A
    mcp48fvxx_channel_on_off(MCP48FVXX_CHANNEL_A, true);

    // Turn off Channel B
    mcp48fvxx_channel_on_off(MCP48FVXX_CHANNEL_B, false);

    return 0;
}
```

## Integration Guide

To port this library to a different platform:

1. Implement `mcp48fvxx_spi_transfer()` in `mcp48fvxx_platform.c` for your hardware.
2. Ensure SPI configuration matches MCP48FVXX requirements (Mode 0, MSB first).
3. Include the appropriate SPI driver headers.

### Example Platform Implementation for ATSAMD21J18 and [MPLABX MCC Harmony](https://github.com/Microchip-MPLAB-Harmony):

```c
#include "mcp48fvxx_platform.h"
#include "peripheral/sercom/spi_master/plib_sercom0_spi_master.h"

uint32_t mcp48fvxx_spi_transfer(uint32_t command_24bit) {
   uint32_t receivedData = 0;
   uint8_t txData[3] = {(command_24bit >> 16) & 0xFF , (command_24bit >> 8) & 0xFF, command_24bit & 0xFF }; 
   uint8_t rxData[3] = {0, 0, 0};

   SERCOM0_SPI_WriteRead(txData, 3, rxData, 3);
        
   receivedData = (rxData[0] << 16) | (rxData[1] << 8) | rxData[2];

   return receivedData;
}
```

## Troubleshooting

### Common Issues

1. **DAC Not Responding**
   - Check SPI connections and chip select timing.
   - Confirm SPI clock frequency is within DAC specifications.

2. **Incorrect Output Voltage**
   - Verify the reference voltage.
   - Check that the correct value is being set (0-4095).
   - Ensure the channel is powered on.

3. **SPI Communication Errors**
   - Check that the SPI mode is correct (typically Mode 0).
   - Verify data format (MSB first).
   - Check for interference on SPI lines.

## References

- [MCP48FVXX Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/20005466A.pdf)

