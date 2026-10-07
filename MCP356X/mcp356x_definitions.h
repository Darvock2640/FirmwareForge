/// \file mcp356x_definitions.h
///
/// This header defines the SPI command and register-map protocol of the
/// MCP356X (2/4/8-channel, 24-bit Delta-Sigma Analog-to-Digital Converter), as described in
/// the "MCP3561/2/4 Family Data Sheet" (Microchip DS20006181C).
///
/// \author Alejandro Beltran
/// \date September 2026

#ifndef MCP356X_DEFINITIONS_H
#define MCP356X_DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup mcp356x_constants 
// Device Constants. Fixed values defined by the MCP356X silicon/marking.
/// \{

#define MCP356X_DEFAULT_DEVICE_ADDRESS  0x01U   ///< Default 2-bit SPI device address hard-coded into the MCP356X.
#define MCP356X_LOCK_WRITE_ENABLE_CODE  0xA5U   ///< Password that unlocks write access to the full register map.
/// Any LOCK[7:0] value other than MCP356X_LOCK_WRITE_ENABLE_CODE locks the register map.
#define MCP356X_LOCK_WRITE_DISABLE_CODE 0x00U   

/// GAINCAL[23:0] reset value, which corresponds to a calibration gain multiplier of exactly 1x.
#define MCP356X_GAINCAL_UNITY           0x800000UL

/// \}

/// \defgroup mcp356x_command 
// Command Byte. Encoding building blocks of the 8-bit COMMAND byte.
/// \{

/// Register addresses.
/// * MCP356X_REG_ADCDATA   
/// * MCP356X_REG_CONFIG0   
/// * MCP356X_REG_CONFIG1   
/// * MCP356X_REG_CONFIG2   
/// * MCP356X_REG_CONFIG3   
/// * MCP356X_REG_IRQ       
/// * MCP356X_REG_MUX       
/// * MCP356X_REG_SCAN      
/// * MCP356X_REG_TIMER     
/// * MCP356X_REG_OFFSETCAL 
/// * MCP356X_REG_GAINCAL   
/// * MCP356X_REG_LOCK      
/// * MCP356X_REG_RESERVED_E
/// * MCP356X_REG_CRCCFG    
typedef enum mcp356x_register
{
    MCP356X_REG_ADCDATA    = 0x00, ///< Latest ADC conversion result.
    MCP356X_REG_CONFIG0    = 0x01, ///< ADC operating mode, clock selection and bias current source, 8 bits.
    MCP356X_REG_CONFIG1    = 0x02, ///< Prescaler and oversampling ratio (OSR), 8 bits.
    MCP356X_REG_CONFIG2    = 0x03, ///< ADC boost, gain and auto-zeroing MUX setting, 8 bits.
    MCP356X_REG_CONFIG3    = 0x04, ///< Conversion mode, data/CRC format and digital calibration enables, 8 bits.
    MCP356X_REG_IRQ        = 0x05, ///< IRQ status flags and IRQ/MDAT pin configuration, 8 bits.
    MCP356X_REG_MUX        = 0x06, ///< Analog multiplexer VIN+/VIN- channel selection, 8 bits.
    MCP356X_REG_SCAN       = 0x07, ///< SCAN mode channel mask and inter-conversion delay, 24 bits.
    MCP356X_REG_TIMER      = 0x08, ///< Delay between SCAN cycles, 24 bits.
    MCP356X_REG_OFFSETCAL  = 0x09, ///< Digital offset calibration code (two's complement), 24 bits.
    MCP356X_REG_GAINCAL    = 0x0A, ///< Digital gain calibration code (unsigned), 24 bits.
    MCP356X_REG_LOCK       = 0x0D, ///< Write-protection password register, 8 bits.
    MCP356X_REG_RESERVED_E = 0x0E, ///< Reserved register, 16 bits. Reading this register returns a device-variant-specific value.
    MCP356X_REG_CRCCFG     = 0x0F  ///< CRC-16 checksum of the register map configuration, 16 bits. Read-only.
} mcp356x_register_t;

/// Command-type.
/// * MCP356X_CMD_TYPE_FAST
/// * MCP356X_CMD_TYPE_STATIC_READ
/// * MCP356X_CMD_TYPE_INCREMENTAL_WRITE
/// * MCP356X_CMD_TYPE_INCREMENTAL_READ
typedef enum mcp356x_cmd_type
{
    MCP356X_CMD_TYPE_FAST              = 0x00, ///< Fast command.
    MCP356X_CMD_TYPE_STATIC_READ       = 0x01, ///< Static read.
    MCP356X_CMD_TYPE_INCREMENTAL_WRITE = 0x02, ///< Incremental write starting at the given register.
    MCP356X_CMD_TYPE_INCREMENTAL_READ  = 0x03  ///< Incremental read starting at the given register.
} mcp356x_cmd_type_t;

/// Fast command codes.
/// * MCP356X_FASTCMD_ADC_START_RESTART
/// * MCP356X_FASTCMD_ADC_STANDBY
/// * MCP356X_FASTCMD_ADC_SHUTDOWN
/// * MCP356X_FASTCMD_FULL_SHUTDOWN
/// * MCP356X_FASTCMD_FULL_RESET
typedef enum mcp356x_fast_cmd
{
    MCP356X_FASTCMD_ADC_START_RESTART = 0x0A, ///< Starts or restarts a conversion.
    MCP356X_FASTCMD_ADC_STANDBY       = 0x0B, ///< Places the ADC in Standby mode.
    MCP356X_FASTCMD_ADC_SHUTDOWN      = 0x0C, ///< Places the ADC in ADC Shutdown mode.
    MCP356X_FASTCMD_FULL_SHUTDOWN     = 0x0D, ///< Places the whole device in Full Shutdown mode.
    MCP356X_FASTCMD_FULL_RESET        = 0x0E  ///< Resets the entire register map to its default state.
} mcp356x_fast_cmd_t;

/// \}

/// STATUS byte returned on SDO while the COMMAND byte is clocked in on SDI.
/// * uint8_t device_addr_ack;
/// * bool    data_ready;
/// * bool    crc_error;
/// * bool    por_occurred;
typedef struct mcp356x_status
{
    uint8_t device_addr_ack; ///< Echo of the hard-coded device address bits, confirming the command was accepted.
    bool    data_ready;      ///< True when a new ADCDATA conversion result is available.
    bool    crc_error;       ///< True when a CRC error has occurred on the register map since the last read.
    bool    por_occurred;    ///< True when a Power-On Reset has occurred since the last read.
} mcp356x_status_t;

/// \defgroup mcp356x_config_enums 
/// Register Field Enumerations. Enumerated values for the writable fields of the CONFIG0-CONFIG3 registers.
/// \{

/// Master clock source selection (CONFIG0).
/// * MCP356X_CLK_SEL_EXTERNAL_DEFAULT
/// * MCP356X_CLK_SEL_EXTERNAL
/// * MCP356X_CLK_SEL_INTERNAL
/// * MCP356X_CLK_SEL_INTERNAL_AMCLK_OUT
typedef enum mcp356x_clk_sel
{
    MCP356X_CLK_SEL_EXTERNAL_DEFAULT   = 0x00, ///< External digital clock on MCLK.
    MCP356X_CLK_SEL_EXTERNAL           = 0x01, ///< External digital clock on MCLK.
    MCP356X_CLK_SEL_INTERNAL           = 0x02, ///< Internal oscillator, no output on the MCLK pin.
    MCP356X_CLK_SEL_INTERNAL_AMCLK_OUT = 0x03  ///< Internal oscillator, AMCLK present on the MCLK pin.
} mcp356x_clk_sel_t;

/// Sensor bias current source/sink selection, sourced on VIN+ and sunk on VIN- (CONFIG0).
/// * MCP356X_BIAS_CURRENT_NONE
/// * MCP356X_BIAS_CURRENT_0_9UA
/// * MCP356X_BIAS_CURRENT_3_7UA
/// * MCP356X_BIAS_CURRENT_15UA
typedef enum mcp356x_bias_current
{
    MCP356X_BIAS_CURRENT_NONE  = 0x00, ///< No bias current applied to the ADC inputs (default).
    MCP356X_BIAS_CURRENT_0_9UA = 0x01, ///< 0.9 uA applied to the ADC inputs.
    MCP356X_BIAS_CURRENT_3_7UA = 0x02, ///< 3.7 uA applied to the ADC inputs.
    MCP356X_BIAS_CURRENT_15UA  = 0x03  ///< 15 uA applied to the ADC inputs.
} mcp356x_bias_current_t;

/// ADC operating mode selection (CONFIG0).
/// * MCP356X_ADC_MODE_SHUTDOWN_DEFAULT
/// * MCP356X_ADC_MODE_SHUTDOWN
/// * MCP356X_ADC_MODE_STANDBY
/// * MCP356X_ADC_MODE_CONVERSION
typedef enum mcp356x_adc_mode
{
    MCP356X_ADC_MODE_SHUTDOWN_DEFAULT = 0x00, ///< ADC Shutdown mode (default).
    MCP356X_ADC_MODE_SHUTDOWN         = 0x01, ///< ADC Shutdown mode.
    MCP356X_ADC_MODE_STANDBY          = 0x02, ///< ADC Standby mode: internal bias circuits stay active.
    MCP356X_ADC_MODE_CONVERSION       = 0x03  ///< ADC Conversion mode: the device converts and updates ADCDATA.
} mcp356x_adc_mode_t;

/// Prescaler applied to MCLK to produce AMCLK (CONFIG1).
/// * MCP356X_PRESCALE_1
/// * MCP356X_PRESCALE_2
/// * MCP356X_PRESCALE_4
/// * MCP356X_PRESCALE_8
typedef enum mcp356x_prescale
{
    MCP356X_PRESCALE_1 = 0x00, ///< AMCLK = MCLK (default).
    MCP356X_PRESCALE_2 = 0x01, ///< AMCLK = MCLK / 2.
    MCP356X_PRESCALE_4 = 0x02, ///< AMCLK = MCLK / 4.
    MCP356X_PRESCALE_8 = 0x03  ///< AMCLK = MCLK / 8.
} mcp356x_prescale_t;

/// Oversampling ratio of the Delta-Sigma modulator/decimation filter (CONFIG1).
/// * MCP356X_OSR_32
/// * MCP356X_OSR_64
/// * MCP356X_OSR_128
/// * MCP356X_OSR_256
/// * MCP356X_OSR_512
/// * MCP356X_OSR_1024
/// * MCP356X_OSR_2048
/// * MCP356X_OSR_4096
/// * MCP356X_OSR_8192
/// * MCP356X_OSR_16384
/// * MCP356X_OSR_20480
/// * MCP356X_OSR_24576
/// * MCP356X_OSR_40960
/// * MCP356X_OSR_49152
/// * MCP356X_OSR_81920
/// * MCP356X_OSR_98304
typedef enum mcp356x_osr
{
    MCP356X_OSR_32    = 0x00,
    MCP356X_OSR_64    = 0x01,
    MCP356X_OSR_128   = 0x02,
    MCP356X_OSR_256   = 0x03, ///< Default.
    MCP356X_OSR_512   = 0x04,
    MCP356X_OSR_1024  = 0x05,
    MCP356X_OSR_2048  = 0x06,
    MCP356X_OSR_4096  = 0x07,
    MCP356X_OSR_8192  = 0x08,
    MCP356X_OSR_16384 = 0x09,
    MCP356X_OSR_20480 = 0x0A,
    MCP356X_OSR_24576 = 0x0B,
    MCP356X_OSR_40960 = 0x0C,
    MCP356X_OSR_49152 = 0x0D,
    MCP356X_OSR_81920 = 0x0E,
    MCP356X_OSR_98304 = 0x0F
} mcp356x_osr_t;

/// ADC bias current selection, scales power consumption and speed (CONFIG2).
/// * MCP356X_BOOST_0_5X
/// * MCP356X_BOOST_0_66X
/// * MCP356X_BOOST_1X
/// * MCP356X_BOOST_2X
typedef enum mcp356x_boost
{
    MCP356X_BOOST_0_5X  = 0x00, ///< ADC channel current x0.5.
    MCP356X_BOOST_0_66X = 0x01, ///< ADC channel current x0.66.
    MCP356X_BOOST_1X    = 0x02, ///< ADC channel current x1 (default).
    MCP356X_BOOST_2X    = 0x03  ///< ADC channel current x2.
} mcp356x_boost_t;

/// Analog/digital front-end gain selection (CONFIG2).
/// * MCP356X_GAIN_1_3X
/// * MCP356X_GAIN_1X
/// * MCP356X_GAIN_2X
/// * MCP356X_GAIN_4X
/// * MCP356X_GAIN_8X
/// * MCP356X_GAIN_16X
/// * MCP356X_GAIN_32X
/// * MCP356X_GAIN_64X
typedef enum mcp356x_gain
{
    MCP356X_GAIN_1_3X = 0x00, ///< Gain is x1/3.
    MCP356X_GAIN_1X   = 0x01, ///< Gain is x1 (default).
    MCP356X_GAIN_2X   = 0x02,
    MCP356X_GAIN_4X   = 0x03,
    MCP356X_GAIN_8X   = 0x04,
    MCP356X_GAIN_16X  = 0x05,
    MCP356X_GAIN_32X  = 0x06, ///< x16 analog, x2 digital.
    MCP356X_GAIN_64X  = 0x07  ///< x16 analog, x4 digital.
} mcp356x_gain_t;

/// Conversion mode selection (CONFIG3).
/// * MCP356X_CONV_MODE_ONE_SHOT_SHUTDOWN_DEFAULT
/// * MCP356X_CONV_MODE_ONE_SHOT_SHUTDOWN
/// * MCP356X_CONV_MODE_ONE_SHOT_STANDBY
/// * MCP356X_CONV_MODE_CONTINUOUS
typedef enum mcp356x_conv_mode
{
    MCP356X_CONV_MODE_ONE_SHOT_SHUTDOWN_DEFAULT = 0x00, ///< One-shot conversion, then ADC_MODE is forced to Shutdown (default).
    MCP356X_CONV_MODE_ONE_SHOT_SHUTDOWN         = 0x01, ///< One-shot conversion, then ADC_MODE is forced to Shutdown.
    MCP356X_CONV_MODE_ONE_SHOT_STANDBY          = 0x02, ///< One-shot conversion, then ADC_MODE is forced to Standby.
    MCP356X_CONV_MODE_CONTINUOUS                = 0x03  ///< Continuous conversion (or continuous SCAN cycling).
} mcp356x_conv_mode_t;

/// ADCDATA output coding selection (CONFIG3).
/// Use with mcp356x_read_adc() to select how many bytes are read and how they are decoded.
/// * MCP356X_DATA_FORMAT_24BIT
/// * MCP356X_DATA_FORMAT_32BIT_LEFT_JUSTIFIED
/// * MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED
/// * MCP356X_DATA_FORMAT_32BIT_CHID_SGN
typedef enum mcp356x_data_format
{
    MCP356X_DATA_FORMAT_24BIT                 = 0x00, ///< 24-bit: SGN + DATA[22:0] (default). No overrange.
    MCP356X_DATA_FORMAT_32BIT_LEFT_JUSTIFIED  = 0x01, ///< 32-bit: SGN + DATA[22:0] left-justified, padded with 0x00. No overrange.
    MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED    = 0x02, ///< 32-bit: 8-bit sign extension + DATA[23:0], right-justified. Allows overrange.
    MCP356X_DATA_FORMAT_32BIT_CHID_SGN        = 0x03  ///< 32-bit: 4-bit Channel ID + 4-bit sign extension + DATA[23:0]. Allows overrange; identifies the source channel in SCAN mode.
} mcp356x_data_format_t;

/// \}

/// MUX_VIN+[3:0] / MUX_VIN-[3:0]: analog multiplexer input selection.
/// * MCP356X_MUX_CH0
/// * MCP356X_MUX_CH1
/// * MCP356X_MUX_CH2
/// * MCP356X_MUX_CH3
/// * MCP356X_MUX_CH4
/// * MCP356X_MUX_CH5
/// * MCP356X_MUX_CH6
/// * MCP356X_MUX_CH7
/// * MCP356X_MUX_AGND
/// * MCP356X_MUX_AVDD
/// * MCP356X_MUX_REFIN_PLUS
/// * MCP356X_MUX_REFIN_MINUS
/// * MCP356X_MUX_TEMP_DIODE_P
/// * MCP356X_MUX_TEMP
/// * MCP356X_MUX_VCM_TEMP_DIODE_M
typedef enum mcp356x_mux_input
{
    MCP356X_MUX_CH0              = 0x00, ///< CH0 (default VIN+).
    MCP356X_MUX_CH1              = 0x01, ///< CH1 (default VIN-).
    MCP356X_MUX_CH2              = 0x02, ///< CH2.
    MCP356X_MUX_CH3              = 0x03, ///< CH3.
    MCP356X_MUX_CH4              = 0x04, ///< CH4.
    MCP356X_MUX_CH5              = 0x05, ///< CH5.
    MCP356X_MUX_CH6              = 0x06, ///< CH6.
    MCP356X_MUX_CH7              = 0x07, ///< CH7.
    MCP356X_MUX_AGND             = 0x08, ///< Analog ground.
    MCP356X_MUX_AVDD             = 0x09, ///< Analog supply voltage.
    MCP356X_MUX_REFIN_PLUS       = 0x0B, ///< REFIN+ pin.
    MCP356X_MUX_REFIN_MINUS      = 0x0C, ///< REFIN- pin.
    MCP356X_MUX_TEMP_DIODE_P     = 0x0D, ///< Internal temperature sensor diode P.
    MCP356X_MUX_TEMP             = 0x0E, ///< Internal temperature sensor (diode P and M pair).
    MCP356X_MUX_VCM_TEMP_DIODE_M = 0x0F  ///< Internal VCM / temperature sensor diode M.
} mcp356x_mux_input_t;

/// \defgroup mcp3562_scan_channels SCAN Register Channel Mask Bits.
/// Bit flags for the @c channel_mask parameter of mcp3562_set_scan() / mcp3562_get_scan().
/// \{
#define MCP356X_SCAN_CH_OFFSET  (1UL << 15) ///< Internal offset (zero-scale) measurement channel.
#define MCP356X_SCAN_CH_VCM     (1UL << 14) ///< Internal common-mode voltage measurement channel.
#define MCP356X_SCAN_CH_AVDD    (1UL << 13) ///< Analog supply voltage measurement channel.
#define MCP356X_SCAN_CH_TEMP    (1UL << 12) ///< Internal temperature sensor channel.
#define MCP356X_SCAN_CH_DIFF_D  (1UL << 11) ///< Differential Channel D (CH6-CH7).
#define MCP356X_SCAN_CH_DIFF_C  (1UL << 10) ///< Differential Channel C (CH4-CH5).
#define MCP356X_SCAN_CH_DIFF_B  (1UL << 9)  ///< Differential Channel B (CH2-CH3).
#define MCP356X_SCAN_CH_DIFF_A  (1UL << 8)  ///< Differential Channel A (CH0-CH1).
#define MCP356X_SCAN_CH_SE_CH7  (1UL << 7)  ///< Single-ended CH7.
#define MCP356X_SCAN_CH_SE_CH6  (1UL << 6)  ///< Single-ended CH6.
#define MCP356X_SCAN_CH_SE_CH5  (1UL << 5)  ///< Single-ended CH5.
#define MCP356X_SCAN_CH_SE_CH4  (1UL << 4)  ///< Single-ended CH4.
#define MCP356X_SCAN_CH_SE_CH3  (1UL << 3)  ///< Single-ended CH3.
#define MCP356X_SCAN_CH_SE_CH2  (1UL << 2)  ///< Single-ended CH2.
#define MCP356X_SCAN_CH_SE_CH1  (1UL << 1)  ///< Single-ended CH1.
#define MCP356X_SCAN_CH_SE_CH0  (1UL << 0)  ///< Single-ended CH0.
/// \}

/// DLY[2:0]: additional settling delay inserted between conversions of a SCAN cycle.
/// * MCP356X_SCAN_DELAY_NONE
/// * MCP356X_SCAN_DELAY_8_DMCLK
/// * MCP356X_SCAN_DELAY_16_DMCLK
/// * MCP356X_SCAN_DELAY_32_DMCLK
/// * MCP356X_SCAN_DELAY_64_DMCLK
/// * MCP356X_SCAN_DELAY_128_DMCLK
/// * MCP356X_SCAN_DELAY_256_DMCLK
/// * MCP356X_SCAN_DELAY_512_DMCLK
typedef enum mcp356x_scan_delay
{
    MCP356X_SCAN_DELAY_NONE       = 0x00, ///< No delay (default).
    MCP356X_SCAN_DELAY_8_DMCLK    = 0x01, ///< 8 x DMCLK periods.
    MCP356X_SCAN_DELAY_16_DMCLK   = 0x02, ///< 16 x DMCLK periods.
    MCP356X_SCAN_DELAY_32_DMCLK   = 0x03, ///< 32 x DMCLK periods.
    MCP356X_SCAN_DELAY_64_DMCLK   = 0x04, ///< 64 x DMCLK periods.
    MCP356X_SCAN_DELAY_128_DMCLK  = 0x05, ///< 128 x DMCLK periods.
    MCP356X_SCAN_DELAY_256_DMCLK  = 0x06, ///< 256 x DMCLK periods.
    MCP356X_SCAN_DELAY_512_DMCLK  = 0x07  ///< 512 x DMCLK periods.
} mcp356x_scan_delay_t;

/// mcp356x_device_variants MCP356X Device Variants.
/// * MCP356X_DEVICE_UNKNOWN
/// * MCP356X_DEVICE_MCP3561
/// * MCP356X_DEVICE_MCP3562
/// * MCP356X_DEVICE_MCP3564
typedef enum mcp356x_device_variant {
    MCP356X_DEVICE_UNKNOWN = 0,     ///< Unknown device variant.
    MCP356X_DEVICE_MCP3561 = 0x0C,  ///< MCP3561: 2-channel, 24-bit ADC.
    MCP356X_DEVICE_MCP3562 = 0x0D,  ///< MCP3562: 4-channel, 24-bit ADC.
    MCP356X_DEVICE_MCP3564 = 0x0F   ///< MCP3564: 8-channel, 24-bit ADC.
}mcp356x_device_variant_t;

/// CONFIG0 register fields.
/// mcp356x_clk_sel_t      clk_sel
/// mcp356x_bias_current_t bias_current
/// mcp356x_adc_mode_t     adc_mode
typedef struct mcp356x_config0
{
    mcp356x_clk_sel_t      clk_sel;       ///< CLK_SEL[1:0]: master clock source.
    mcp356x_bias_current_t bias_current;  ///< CS_SEL[1:0]: sensor bias current source/sink.
    mcp356x_adc_mode_t     adc_mode;      ///< ADC_MODE[1:0]: ADC operating mode.
} mcp356x_config0_t;

/// CONFIG1 register fields.
/// * mcp356x_prescale_t prescale
/// * mcp356x_osr_t       osr
typedef struct mcp356x_config1
{
    mcp356x_prescale_t prescale; ///< PRE[1:0]: MCLK-to-AMCLK prescaler.
    mcp356x_osr_t       osr;     ///< OSR[3:0]: oversampling ratio.
} mcp356x_config1_t;

/// CONFIG2 register fields.
/// mcp356x_boost_t boost
/// mcp356x_gain_t  gain
/// bool            az_mux_enable
typedef struct mcp356x_config2
{
    mcp356x_boost_t boost;         ///< BOOST[1:0]: ADC bias current selection.
    mcp356x_gain_t  gain;          ///< GAIN[2:0]: front-end gain selection.
    bool            az_mux_enable; ///< AZ_MUX: enables the analog input MUX auto-zeroing algorithm (doubles conversion time).
} mcp356x_config2_t;

/// CONFIG3 register fields.
/// * mcp356x_conv_mode_t   conv_mode
/// * mcp356x_data_format_t data_format
/// * bool                  crc_format_32bit
/// * bool                  crc_com_enable
/// * bool                  offset_cal_enable
/// * bool                  gain_cal_enable
typedef struct mcp356x_config3
{
    mcp356x_conv_mode_t   conv_mode;         ///< CONV_MODE[1:0]: conversion mode selection.
    mcp356x_data_format_t data_format;       ///< DATA_FORMAT[1:0]: ADCDATA output coding.
    bool                  crc_format_32bit;  ///< CRC_FORMAT: true selects 32-bit (CRC-16 + 16 zero bits) on read communications, false selects 16-bit.
    bool                  crc_com_enable;    ///< EN_CRCCOM: enables the CRC-16 checksum appended to read communications.
    bool                  offset_cal_enable; ///< EN_OFFCAL: enables digital offset calibration (OFFSETCAL).
    bool                  gain_cal_enable;   ///< EN_GAINCAL: enables digital gain calibration (GAINCAL).
} mcp356x_config3_t;

/// IRQ register fields: writable configuration plus the latched status flags.
/// * bool mdat_output_enable
/// * bool irq_inactive_high
/// * bool fast_cmd_enable
/// * bool conv_start_irq_enable
/// * bool data_ready
/// * bool crc_error
/// * bool por_occurred
typedef struct mcp356x_irq
{
    bool mdat_output_enable;   ///< IRQ_MODE[1]: true routes the modulator output stream (MDAT) to the IRQ/MDAT pin instead of interrupts.
    bool irq_inactive_high;    ///< IRQ_MODE[0]: true sets the inactive state of the IRQ pin to logic high; false requires an external pull-up (default).
    bool fast_cmd_enable;      ///< EN_FASTCMD: enables Fast commands in the COMMAND byte (default true).
    bool conv_start_irq_enable;///< EN_STP: enables the conversion-start pulse on the interrupt output (default true).
    bool data_ready;           ///< DR_STATUS (read-only): true when a new ADCDATA conversion result is available.
    bool crc_error;            ///< CRCCFG_STATUS (read-only): true when a register-map CRC error has occurred.
    bool por_occurred;         ///< POR_STATUS (read-only): true when a Power-On Reset has occurred since the last read.
} mcp356x_irq_t;

#ifdef __cplusplus
}
#endif

#endif /* MCP356X_DEFINITIONS_H */