# MCP356X Library Documentation

## Overview

The MCP356X library provides a hardware-agnostic interface for Microchip's MCP3561/2/4 family of
24-bit Delta-Sigma Analog-to-Digital Converters (ADC) over SPI. It implements the device's COMMAND
byte / register-map protocol, as described in the "MCP3561/2/4 Family Data Sheet" (Microchip
DS20006181C), and provides a simple API to configure the ADC, start conversions and read results.

The library is designed to drive **multiple MCP356X devices** on the same SPI bus. For this reason,
**the library does not control the Chip Select (CS) line**. The application must assert and deassert
the CS pin of the target device around every library call. See [Chip Select Handling](#chip-select-handling).

## Hardware Information

The MCP356X family includes 24-bit Delta-Sigma ADCs with the following features:
- 24-bit resolution, programmable data rate up to 153.6 ksps
- Programmable oversampling ratio (OSR 32 to 98304) and gain (1/3x to 64x)
- Single-ended or differential inputs, plus internal temperature sensor, AVDD, VCM and offset channels
- Internal sequencer (SCAN mode) to cycle through multiple channels automatically
- Digital offset and gain calibration registers
- SPI interface, Modes 0,0 and 1,1, up to 20 MHz
- Dedicated IRQ/MDAT pin, CRC-16 protection and register-map write lock

### Device Types

The library supports the following variants:
- MCP3561: 2 single-ended channels / 1 differential pair
- MCP3562: 4 single-ended channels / 2 differential pairs
- MCP3564: 8 single-ended channels / 4 differential pairs

### Pin Configuration

| Pin | Description |
|-----|-------------|
| SDI | SPI Data Input |
| SDO | SPI Data Output |
| SCK | SPI Clock Input |
| CS | Chip Select (active low) - **controlled by the application** |
| IRQ/MDAT | Interrupt output / modulator data output |
| MCLK | Master clock input, or clock output when using the internal oscillator |
| REFIN+ / REFIN- | Differential voltage reference inputs |
| CH0 - CH7 | Analog input channels (count depends on the variant) |
| AVDD / AGND / DVDD / DGND | Supplies and grounds |

## Library Architecture

The library is organized into the following files:

1. **mcp356x_definitions.h**: Register addresses, command codes, enumerations and structures that
   describe the device registers.
2. **mcp356x_api.h**: Main API header file defining the interface functions.
3. **mcp356x_api.c**: Implementation of the API functions. Contains no microcontroller-specific code.
4. **mcp356x_platform.h**: Platform-specific interface declaration (`mcp356x_spi_transfer()`).
5. **mcp356x_platform.c**: Platform-specific implementation of the SPI transfer.

This separation allows porting to different hardware platforms by only modifying the platform-specific
files while keeping the API unchanged.

## Chip Select Handling

The MCP356X starts a new SPI command on each CS falling edge and ends it on the CS rising edge. Since
the library is agnostic and supports multiple devices, it does not know which GPIO is connected to each
device. Therefore:

- The library **never** drives the CS pin. Neither the API layer nor `mcp356x_spi_transfer()` touches CS.
- The application must **assert CS (drive low)** before each library call and **deassert CS (drive high)**
  right after it returns.
- Every API function performs **exactly one** SPI transaction, so each call must be wrapped by its own
  CS assert / deassert pair. Do not wrap several calls in a single CS cycle: the device expects a new
  COMMAND byte only after a CS falling edge.
- Keep all CS pins high (deasserted) at startup, before the first library call.

```c
GPIO_PA07_Clear();                                          // Assert CS of ADC 1
mcp356x_start_conversion(MCP356X_DEFAULT_DEVICE_ADDRESS);
GPIO_PA07_Set();                                            // Deassert CS of ADC 1
```

> `GPIO_PA07_Clear()` / `GPIO_PA07_Set()` are MPLAB Harmony pin macros. Replace them with the GPIO
> functions of your platform.

## API Reference

All functions take a `device_address` parameter. This is the 2-bit SPI device address hard-coded in
the MCP356X (set by the part number ordering option). The default value is
`MCP356X_DEFAULT_DEVICE_ADDRESS` (`0x01`). It is included in every COMMAND byte; the device only
responds when it matches its own address.

Unless stated otherwise, functions return `true` if the SPI transaction completed and `false` on an
SPI transfer failure or an invalid argument (for example, a `NULL` pointer).

### Device Identification

```c
bool mcp356x_identify(uint8_t device_address, mcp356x_device_variant_t *variant);
```

**Description**: Reads the device ID and reports which variant is connected. Also works as a
device-presence check.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `variant`: Receives the detected variant: `MCP356X_DEVICE_MCP3561`, `MCP356X_DEVICE_MCP3562`,
  `MCP356X_DEVICE_MCP3564` or `MCP356X_DEVICE_UNKNOWN`

**Returns**:
- `true` if the SPI transaction completed (even if the variant is unknown)
- `false` otherwise

> A result of `MCP356X_DEVICE_UNKNOWN` means the device is not responding or is not an MCP3561/2/4.

### Fast Commands

```c
bool mcp356x_reset(uint8_t device_address);
bool mcp356x_start_conversion(uint8_t device_address);
bool mcp356x_standby(uint8_t device_address);
bool mcp356x_adc_shutdown(uint8_t device_address);
bool mcp356x_full_shutdown(uint8_t device_address);
```

**Description**: Single-byte commands that change the device state immediately.

| Function | Action |
|----------|--------|
| `mcp356x_reset()` | Resets all registers to their power-on default values |
| `mcp356x_start_conversion()` | Starts or restarts a conversion (ADC mode = Conversion) |
| `mcp356x_standby()` | Places the ADC in Standby mode |
| `mcp356x_adc_shutdown()` | Places the ADC in Shutdown mode |
| `mcp356x_full_shutdown()` | Places the whole device in Full Shutdown mode (CONFIG0 = 0x00) |

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device

**Returns**:
- `true` if successful
- `false` otherwise

> Fast commands require `fast_cmd_enable = true` in the IRQ register (default value).

### Configuration Registers (CONFIG0 - CONFIG3)

```c
bool mcp356x_set_config0(uint8_t device_address, const mcp356x_config0_t *config);
bool mcp356x_get_config0(uint8_t device_address, mcp356x_config0_t *config);

bool mcp356x_set_config1(uint8_t device_address, const mcp356x_config1_t *config);
bool mcp356x_get_config1(uint8_t device_address, mcp356x_config1_t *config);

bool mcp356x_set_config2(uint8_t device_address, const mcp356x_config2_t *config);
bool mcp356x_get_config2(uint8_t device_address, mcp356x_config2_t *config);

bool mcp356x_set_config3(uint8_t device_address, const mcp356x_config3_t *config);
bool mcp356x_get_config3(uint8_t device_address, mcp356x_config3_t *config);
```

**Description**: Write or read the four configuration registers using typed structures.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `config`: Structure to write (set) or to fill with the current values (get)

**Structure fields**:

| Structure | Field | Values | Description |
|-----------|-------|--------|-------------|
| `mcp356x_config0_t` | `clk_sel` | `MCP356X_CLK_SEL_*` | Clock source: external, internal, or internal with clock output on MCLK |
| | `bias_current` | `MCP356X_BIAS_CURRENT_*` | Sensor bias current: none, 0.9 uA, 3.7 uA or 15 uA |
| | `adc_mode` | `MCP356X_ADC_MODE_*` | Shutdown, Standby or Conversion |
| `mcp356x_config1_t` | `prescale` | `MCP356X_PRESCALE_1/2/4/8` | MCLK divider |
| | `osr` | `MCP356X_OSR_32` ... `MCP356X_OSR_98304` | Oversampling ratio (default 256) |
| `mcp356x_config2_t` | `boost` | `MCP356X_BOOST_0_5X/0_66X/1X/2X` | ADC bias current |
| | `gain` | `MCP356X_GAIN_1_3X` ... `MCP356X_GAIN_64X` | Front-end gain (default 1x) |
| | `az_mux_enable` | `true` / `false` | Input MUX auto-zeroing (doubles conversion time) |
| `mcp356x_config3_t` | `conv_mode` | `MCP356X_CONV_MODE_*` | One-shot then Shutdown, one-shot then Standby, or Continuous |
| | `data_format` | `MCP356X_DATA_FORMAT_*` | ADC data output format (see [Reading Conversion Results](#reading-conversion-results)) |
| | `crc_format_32bit` | `true` / `false` | CRC format: 32-bit (`true`) or 16-bit (`false`) |
| | `crc_com_enable` | `true` / `false` | Appends a CRC-16 checksum to read communications |
| | `offset_cal_enable` | `true` / `false` | Enables digital offset calibration (OFFSETCAL) |
| | `gain_cal_enable` | `true` / `false` | Enables digital gain calibration (GAINCAL) |

**Returns**:
- `true` if successful
- `false` otherwise

> Setting `adc_mode = MCP356X_ADC_MODE_CONVERSION` in CONFIG0 starts conversions immediately. Write
> CONFIG0 last if you want the other registers configured before the first conversion.

### Interrupt Register (IRQ)

```c
bool mcp356x_set_irq(uint8_t device_address, const mcp356x_irq_t *config);
bool mcp356x_get_irq(uint8_t device_address, mcp356x_irq_t *config);
```

**Description**: Write the IRQ/MDAT pin configuration, or read it back together with the latched
status flags.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `config`: Structure to write (set) or to fill (get). Fields:
  - `mdat_output_enable`: `true` outputs the modulator data (MDAT) on the IRQ/MDAT pin instead of interrupts
  - `irq_inactive_high`: `true` drives the IRQ pin high when inactive; `false` requires an external pull-up (default)
  - `fast_cmd_enable`: Enables Fast commands (default `true`)
  - `conv_start_irq_enable`: Enables the conversion-start pulse on the IRQ pin (default `true`)
  - `data_ready`, `crc_error`, `por_occurred`: Read-only status flags, ignored by `mcp356x_set_irq()`

**Returns**:
- `true` if successful
- `false` otherwise

### Input Multiplexer (MUX)

```c
bool mcp356x_set_mux(uint8_t device_address, mcp356x_mux_input_t vin_plus, mcp356x_mux_input_t vin_minus);
bool mcp356x_get_mux(uint8_t device_address, mcp356x_mux_input_t *vin_plus, mcp356x_mux_input_t *vin_minus);
```

**Description**: Selects (or reads back) the inputs connected to the positive and negative ADC inputs.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `vin_plus`: Input connected to VIN+
- `vin_minus`: Input connected to VIN-

Available inputs: `MCP356X_MUX_CH0` ... `MCP356X_MUX_CH7`, `MCP356X_MUX_AGND`, `MCP356X_MUX_AVDD`,
`MCP356X_MUX_REFIN_PLUS`, `MCP356X_MUX_REFIN_MINUS`, `MCP356X_MUX_TEMP_DIODE_P`, `MCP356X_MUX_TEMP`,
`MCP356X_MUX_VCM_TEMP_DIODE_M`.

**Returns**:
- `true` if successful
- `false` otherwise

> For a single-ended measurement, set `vin_minus` to `MCP356X_MUX_AGND`.

### SCAN Mode

```c
bool mcp356x_set_scan(uint8_t device_address, uint16_t channel_mask, mcp356x_scan_delay_t delay);
bool mcp356x_get_scan(uint8_t device_address, uint16_t *channel_mask, mcp356x_scan_delay_t *delay);

bool mcp356x_set_timer(uint8_t device_address, uint32_t delay_counts);
bool mcp356x_get_timer(uint8_t device_address, uint32_t *delay_counts);
```

**Description**: Configures the internal sequencer, which converts a list of channels automatically.
`mcp356x_set_scan()` selects the channels and the delay between conversions. `mcp356x_set_timer()`
sets the delay between two complete SCAN cycles.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `channel_mask`: Bitwise OR of `MCP356X_SCAN_CH_*` flags:
  - Single-ended: `MCP356X_SCAN_CH_SE_CH0` ... `MCP356X_SCAN_CH_SE_CH7`
  - Differential: `MCP356X_SCAN_CH_DIFF_A` (CH0-CH1) ... `MCP356X_SCAN_CH_DIFF_D` (CH6-CH7)
  - Internal: `MCP356X_SCAN_CH_TEMP`, `MCP356X_SCAN_CH_AVDD`, `MCP356X_SCAN_CH_VCM`, `MCP356X_SCAN_CH_OFFSET`
- `delay`: Delay between conversions, `MCP356X_SCAN_DELAY_NONE` ... `MCP356X_SCAN_DELAY_512_DMCLK`
- `delay_counts`: 24-bit delay between SCAN cycles, in DMCLK periods (0 = no delay)

**Returns**:
- `true` if successful
- `false` otherwise

> Writing a non-zero `channel_mask` enables SCAN mode; the MUX setting is then ignored. Use
> `MCP356X_DATA_FORMAT_32BIT_CHID_SGN` to know which channel produced each result.

### Digital Calibration

```c
bool mcp356x_set_offset_calibration(uint8_t device_address, int32_t offset_code);
bool mcp356x_get_offset_calibration(uint8_t device_address, int32_t *offset_code);

bool mcp356x_set_gain_calibration(uint8_t device_address, uint32_t gain_code);
bool mcp356x_get_gain_calibration(uint8_t device_address, uint32_t *gain_code);
```

**Description**: Sets or reads the digital calibration values applied to each conversion result.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `offset_code`: 24-bit signed offset added to the result, range -8388608 to 8388607
- `gain_code`: 24-bit unsigned gain multiplier. `MCP356X_GAINCAL_UNITY` (`0x800000`) = 1x

**Returns**:
- `true` if successful
- `false` otherwise

> Calibration is applied only when `offset_cal_enable` / `gain_cal_enable` are set in CONFIG3.

### Register Lock and CRC

```c
bool mcp356x_lock(uint8_t device_address);
bool mcp356x_unlock(uint8_t device_address);
bool mcp356x_get_crc(uint8_t device_address, uint16_t *crc);
```

**Description**:
- `mcp356x_lock()`: Blocks writes to all registers except the LOCK register.
- `mcp356x_unlock()`: Restores write access to all registers.
- `mcp356x_get_crc()`: Reads the CRC-16 checksum of the register configuration. The device updates
  it continuously while the registers are locked.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `crc`: Receives the 16-bit checksum

**Returns**:
- `true` if successful
- `false` otherwise

### Reading Conversion Results

```c
bool     mcp356x_is_data_ready(uint8_t device_address);
uint32_t mcp356x_read_adc(uint8_t device_address, mcp356x_data_format_t format, uint8_t *channel_id);
float    mcp356x_convert_to_voltage(int32_t code, float vref, mcp356x_gain_t gain);
```

#### mcp356x_is_data_ready

**Description**: Checks whether a new conversion result is available. Uses a single-byte poll that
does not read any register.

**Returns**:
- `true` if a new result is ready
- `false` if no new result is ready, or the SPI transfer failed

#### mcp356x_read_adc

**Description**: Reads and decodes the latest conversion result.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `format`: Must match the `data_format` set in CONFIG3:

  | Format | Bytes read | Description |
  |--------|-----------|-------------|
  | `MCP356X_DATA_FORMAT_24BIT` | 3 | 24-bit signed result (default) |
  | `MCP356X_DATA_FORMAT_32BIT_LEFT_JUSTIFIED` | 4 | 24-bit result left-justified in 32 bits |
  | `MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED` | 4 | 24-bit result with sign extension; allows overrange |
  | `MCP356X_DATA_FORMAT_32BIT_CHID_SGN` | 4 | Same as above, plus the 4-bit SCAN channel ID |

- `channel_id`: Optional. With `MCP356X_DATA_FORMAT_32BIT_CHID_SGN`, receives the channel ID of the
  result. Pass `NULL` if not needed.

**Returns**:
- The conversion result as raw code.

#### mcp356x_convert_to_voltage

**Description**: Converts a conversion result to volts: `VIN = (code / 8388608) * (vref / gain)`.
This function does not use SPI and needs no CS handling.

**Parameters**:
- `code`: Signed result returned by `mcp356x_read_adc()`
- `vref`: Reference voltage in volts (REFIN+ - REFIN-)
- `gain`: Gain configured in CONFIG2 when the result was acquired

**Returns**:
- The input voltage in volts

### Lower-Level Operations

```c
bool mcp356x_write_register(uint8_t device_address, mcp356x_register_t reg, const uint8_t *data,
                            uint8_t length, mcp356x_status_t *status);
bool mcp356x_read_register(uint8_t device_address, mcp356x_register_t reg, uint8_t *data,
                           uint8_t length, mcp356x_status_t *status);
bool mcp356x_fast_command(uint8_t device_address, mcp356x_fast_cmd_t command, mcp356x_status_t *status);
bool mcp356x_get_status(uint8_t device_address, mcp356x_status_t *status);
```

**Description**: Direct register and command access for advanced use. All higher-level functions are
built on these.

**Parameters**:
- `device_address`: 2-bit SPI device address of the target device
- `reg`: Register address (`MCP356X_REG_*`)
- `data`: Buffer with the bytes to write, or to receive the bytes read, most-significant byte first
- `length`: Number of bytes: 1 to 3 for writes, 1 to 4 for reads (4 only for ADCDATA in a 32-bit format)
- `command`: Fast command code (`MCP356X_FASTCMD_*`)
- `status`: Optional (except for `mcp356x_get_status()`). Receives the STATUS byte returned by the
  device during the transaction: `data_ready`, `crc_error`, `por_occurred` and `device_addr_ack`.
  Pass `NULL` if not needed.

**Returns**:
- `true` if successful
- `false` on SPI transfer failure or invalid argument

## Usage Examples

### Example 1: Single-Ended One-Shot Measurement

```c
#include "mcp356x_api.h"

#define ADC_ADDR        MCP356X_DEFAULT_DEVICE_ADDRESS
#define ADC_CS_ASSERT()   GPIO_PA07_Clear()
#define ADC_CS_DEASSERT() GPIO_PA07_Set()
#define ADC_VREF        3.3f

int main(void) {
    ADC_CS_DEASSERT();   // CS idle high before any transaction

    // Verify the device
    mcp356x_device_variant_t variant;
    ADC_CS_ASSERT();
    mcp356x_identify(ADC_ADDR, &variant);
    ADC_CS_DEASSERT();
    if (variant == MCP356X_DEVICE_UNKNOWN) {
        return -1;
    }

    // Configure: gain 1x, one-shot conversions, 32-bit sign-extended output
    mcp356x_config2_t config2 = {
        .boost = MCP356X_BOOST_1X,
        .gain = MCP356X_GAIN_1X,
        .az_mux_enable = false,
    };
    ADC_CS_ASSERT();
    mcp356x_set_config2(ADC_ADDR, &config2);
    ADC_CS_DEASSERT();

    mcp356x_config3_t config3 = {
        .conv_mode = MCP356X_CONV_MODE_ONE_SHOT_STANDBY,
        .data_format = MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED,
    };
    ADC_CS_ASSERT();
    mcp356x_set_config3(ADC_ADDR, &config3);
    ADC_CS_DEASSERT();

    // Internal clock, ADC in Standby (written last)
    mcp356x_config0_t config0 = {
        .clk_sel = MCP356X_CLK_SEL_INTERNAL,
        .bias_current = MCP356X_BIAS_CURRENT_NONE,
        .adc_mode = MCP356X_ADC_MODE_STANDBY,
    };
    ADC_CS_ASSERT();
    mcp356x_set_config0(ADC_ADDR, &config0);
    ADC_CS_DEASSERT();

    // Measure CH3 against AGND
    ADC_CS_ASSERT();
    mcp356x_set_mux(ADC_ADDR, MCP356X_MUX_CH3, MCP356X_MUX_AGND);
    ADC_CS_DEASSERT();

    ADC_CS_ASSERT();
    mcp356x_start_conversion(ADC_ADDR);
    ADC_CS_DEASSERT();

    bool ready = false;
    while (!ready) {
        ADC_CS_ASSERT();
        ready = mcp356x_is_data_ready(ADC_ADDR);
        ADC_CS_DEASSERT();
    }

    ADC_CS_ASSERT();
    int32_t code = (int32_t)mcp356x_read_adc(ADC_ADDR, MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED, NULL);
    ADC_CS_DEASSERT();

    float volts = mcp356x_convert_to_voltage(code, ADC_VREF, MCP356X_GAIN_1X);
    (void)volts;

    return 0;
}
```

### Example 2: Two Devices on the Same SPI Bus

Both devices can use the same device address, because the CS line selects which one responds. A
small wrapper keeps the CS handling in one place.

```c
#include "mcp356x_api.h"

typedef struct {
    void (*cs_assert)(void);
    void (*cs_deassert)(void);
    uint8_t address;
} adc_t;

static void adc1_cs_low(void)  { GPIO_PA07_Clear(); }
static void adc1_cs_high(void) { GPIO_PA07_Set(); }
static void adc2_cs_low(void)  { GPIO_PA08_Clear(); }
static void adc2_cs_high(void) { GPIO_PA08_Set(); }

static const adc_t adc1 = { adc1_cs_low, adc1_cs_high, MCP356X_DEFAULT_DEVICE_ADDRESS };
static const adc_t adc2 = { adc2_cs_low, adc2_cs_high, MCP356X_DEFAULT_DEVICE_ADDRESS };

static int32_t adc_read(const adc_t *adc, mcp356x_mux_input_t channel) {
    bool ready = false;
    int32_t code;

    adc->cs_assert();
    mcp356x_set_mux(adc->address, channel, MCP356X_MUX_AGND);
    adc->cs_deassert();

    adc->cs_assert();
    mcp356x_start_conversion(adc->address);
    adc->cs_deassert();

    while (!ready) {
        adc->cs_assert();
        ready = mcp356x_is_data_ready(adc->address);
        adc->cs_deassert();
    }

    adc->cs_assert();
    code = (int32_t)mcp356x_read_adc(adc->address, MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED, NULL);
    adc->cs_deassert();

    return code;
}

void read_both(void) {
    int32_t a = adc_read(&adc1, MCP356X_MUX_CH0);
    int32_t b = adc_read(&adc2, MCP356X_MUX_CH0);
    (void)a;
    (void)b;
}
```

## SPI Communication Protocol

Each MCP356X transaction uses the following sequence:

1. The application drives CS low (CS falling edge starts a new command)
2. Master sends the COMMAND byte on SDI:
   - Bits [7:6]: device address
   - Bits [5:2]: register address or Fast command code
   - Bits [1:0]: command type (Fast, Static Read, Incremental Write, Incremental Read)
3. At the same time, the device returns the STATUS byte on SDO
4. For writes: master sends the data bytes, most-significant byte first
5. For reads: master clocks out dummy bytes while the device returns the data, most-significant byte first
6. The application drives CS high (CS rising edge ends the command)

Steps 2 to 5 are performed by the library. Steps 1 and 6 are the application's responsibility.

## Integration Guide

To port this library to a different platform:

1. Modify `mcp356x_platform.c` to implement `mcp356x_spi_transfer()` for your hardware:
   - Send `length` bytes from `tx_buffer` while receiving `length` bytes into `rx_buffer` (full-duplex).
   - The transfer must be **blocking**: it must not return until all bytes are exchanged, because the
     application deasserts CS right after the library call returns.
   - Do **not** drive CS in this function.
   - Return `true` if the transfer completed, `false` otherwise.
2. Configure the SPI peripheral as master, 8-bit, MSB first, SPI Mode 0,0 or 1,1, clock up to 20 MHz.
3. Configure one GPIO output per MCP356X device as CS, and set it high at startup.

The bundled implementation targets a Microchip SAM device with SERCOM0 configured as an SPI master
through MPLAB Harmony (MCC), using `SERCOM0_SPI_WriteRead()` in blocking (non-interrupt) mode.

## Troubleshooting

### Common Issues:

1. **Device not responding / `MCP356X_DEVICE_UNKNOWN`**: Check that the application asserts the
   correct CS pin around each call, and that `device_address` matches the part's hard-coded address.
2. **Corrupted or shifted data**: Make sure CS is toggled around every single library call. Wrapping
   several calls in one CS cycle breaks the protocol.
3. **Data ready never becomes true**: Check that a conversion was started (`mcp356x_start_conversion()`
   or `adc_mode = MCP356X_ADC_MODE_CONVERSION`) and that the clock source in CONFIG0 is valid.
4. **Wrong conversion values**: Make sure the `format` passed to `mcp356x_read_adc()` matches the
   `data_format` set in CONFIG3, and that `vref` and `gain` passed to `mcp356x_convert_to_voltage()`
   match the hardware and CONFIG2.
5. **Fast commands ignored**: Check that `fast_cmd_enable` is `true` in the IRQ register.
6. **Register writes ignored**: The register map may be locked. Call `mcp356x_unlock()`.

## References

- [MCP3561/2/4 Family Data Sheet (DS20006181C)](https://www.microchip.com/en-us/product/MCP3562)
