/// \file mcp356x_api.h
/// Hardware-agnostic API for the Microchip MCP356X 24-bit Delta-Sigma ADC.
///
/// This header defines the API functions for using the
/// MCP356X (2/4/8-channel, 24-bit Delta-Sigma Analog-to-Digital Converter). It is split from the
/// platform layer (see mcp356x_platform.h) so this file and its implementation contain
/// no microcontroller-specific code: every function operates purely on the MCP356X
/// COMMAND byte / register-map protocol and calls into mcp356x_spi_transfer() to move
/// bytes over the wire.
///
/// \author Alejandro Beltran
/// \date September 2026

#ifndef MCP356X_API_H
#define MCP356X_API_H

#include <stdint.h>
#include <stdbool.h>
#include "mcp356x_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup mcp356x_register_access Low-Level Register and Command Access
/// Direct COMMAND byte level operations. Higher-level helpers below are built on these.
/// \{

/// Issues an Incremental Write starting at a register address.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \param reg Register to write; \p length must match (or be less than or equal to) its width.
/// \param data Buffer holding \p length bytes to write, most-significant byte first.
/// \param length Number of bytes to write.
/// \param status Optional; if non-NULL, receives the STATUS byte captured during this transaction.
/// \return true if the SPI transaction completed, false on a platform transfer failure or invalid argument.
bool mcp356x_write_register(uint8_t device_address, mcp356x_register_t reg, const uint8_t *data, uint8_t length, 
                            mcp356x_status_t *status);

/// Issues a Static Read of a single register address.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \param reg Register to read; \p length must match (or be less than or equal to) its width.
/// \param data Buffer to receive \p length bytes, most-significant byte first.
/// \param length Number of bytes to read (1-4; 4 only valid for ::MCP3562_REG_ADCDATA in a 32-bit format).
/// \param status Optional; if non-NULL, receives the STATUS byte captured during this transaction.
/// \return true if the SPI transaction completed, false on a platform transfer failure or invalid argument.
bool mcp356x_read_register(uint8_t device_address, mcp356x_register_t reg, uint8_t *data, uint8_t length, 
                           mcp356x_status_t *status);

/// Issues a single-byte Fast command.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \param command Fast command to execute.
/// \param status Optional; if non-NULL, receives the STATUS byte captured during this transaction.
/// \return true if the SPI transaction completed, false on a platform transfer failure.
bool mcp356x_fast_command(uint8_t device_address, mcp356x_fast_cmd_t command, mcp356x_status_t *status);

/// Retrieves the STATUS byte without touching any register, using the "Don't Care" fast command.
/// \details Useful for cheaply polling ::mcp3562_status_t::data_ready without reading the IRQ register.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \param status Receives the decoded STATUS byte. Must not be NULL.
/// \return true if the SPI transaction completed, false on a platform transfer failure.
bool mcp356x_get_status(uint8_t device_address, mcp356x_status_t *status);

/// \}

/// @defgroup mcp356x_fast_commands Fast Command Helpers.
/// Convenience wrappers over mcp356x_fast_command().
/// \{

/// Resets the entire register map to its default (POR) state. \return true on success.
bool mcp356x_reset(uint8_t device_address);

/// Starts, or restarts, an ADC conversion (forces ADC_MODE[1:0] = 11). \return true on success.
bool mcp356x_start_conversion(uint8_t device_address);

/// Places the ADC in Standby mode (forces ADC_MODE[1:0] = 10). \return true on success.
bool mcp356x_standby(uint8_t device_address);

/// Places the ADC in Shutdown mode (forces ADC_MODE[1:0] = 00). \return true on success.
bool mcp356x_adc_shutdown(uint8_t device_address);

/// Places the entire device in Full Shutdown mode (forces CONFIG0 = 0x00). \return true on success.
bool mcp356x_full_shutdown(uint8_t device_address);

/// \}

/// \defgroup mcp356x_registers Typed Register Accessors.
/// Read/write helpers that pack and unpack the enumerations and structs above into their register encoding.
/// \{

/// Writes CONFIG0. \return true on success.
bool mcp356x_set_config0(uint8_t device_address, const mcp356x_config0_t *config);
/// Reads CONFIG0. \return true on success.
bool mcp356x_get_config0(uint8_t device_address, mcp356x_config0_t *config);

/// Writes CONFIG1. \return true on success.
bool mcp356x_set_config1(uint8_t device_address, const mcp356x_config1_t *config);
/// Reads CONFIG1. \return true on success.
bool mcp356x_get_config1(uint8_t device_address, mcp356x_config1_t *config);

/// Writes CONFIG2. \return true on success.
bool mcp356x_set_config2(uint8_t device_address, const mcp356x_config2_t *config);
/// Reads CONFIG2. \return true on success.
bool mcp356x_get_config2(uint8_t device_address, mcp356x_config2_t *config);

/// Writes CONFIG3. \return true on success.
bool mcp356x_set_config3(uint8_t device_address, const mcp356x_config3_t *config);
/// Reads CONFIG3. \return true on success.
bool mcp356x_get_config3(uint8_t device_address, mcp356x_config3_t *config);

/// Writes the configuration fields of IRQ (status fields in @p config are ignored). \return true on success.
bool mcp356x_set_irq(uint8_t device_address, const mcp356x_irq_t *config);
/// Reads IRQ, including its latched status flags. \return true on success.
bool mcp356x_get_irq(uint8_t device_address, mcp356x_irq_t *config);

/// Selects the analog multiplexer input pair used in MUX mode.
/// \param vin_plus Input routed to VIN+.
/// \param vin_minus Input routed to VIN-.
/// \return true on success.
bool mcp356x_set_mux(uint8_t device_address, mcp356x_mux_input_t vin_plus, mcp356x_mux_input_t vin_minus);

/// Reads back the current MUX VIN+/VIN- selection. \return true on success.
bool mcp356x_get_mux(uint8_t device_address, mcp356x_mux_input_t *vin_plus, mcp356x_mux_input_t *vin_minus);

/// Configures SCAN mode: which channels are converted and the inter-conversion delay.
/// \param channel_mask Bitwise OR of `MCP356x_SCAN_CH_*` flags selecting the channels to sequence through.
/// \param delay Additional settling delay inserted between conversions.
/// \return true on success.
bool mcp356x_set_scan(uint8_t device_address, uint16_t channel_mask, mcp356x_scan_delay_t delay);

/// Reads back the current SCAN channel mask and delay. \return true on success.
bool mcp356x_get_scan(uint8_t device_address, uint16_t *channel_mask, mcp356x_scan_delay_t *delay);

/// Sets the delay between consecutive SCAN cycles (TIMER register), in DMCLK periods.
/// \param delay_counts 24-bit delay count (0 = no delay).
/// \return true on success.
bool mcp356x_set_timer(uint8_t device_address, uint32_t delay_counts);

/// Reads back the TIMER delay count. \return true on success.
bool mcp356x_get_timer(uint8_t device_address, uint32_t *delay_counts);

/// Sets the digital offset calibration code (OFFSETCAL), added to ADCDATA when enabled via CONFIG3::offset_cal_enable.
/// \param offset_code 24-bit two's complement offset code, range -8388608 to 8388607.
/// \return true on success.
bool mcp356x_set_offset_calibration(uint8_t device_address, int32_t offset_code);

/// Reads back the OFFSETCAL code, sign-extended to a 32-bit value. \return true on success.
bool mcp356x_get_offset_calibration(uint8_t device_address, int32_t *offset_code);

/// Sets the digital gain calibration code (GAINCAL), applied to ADCDATA when enabled via CONFIG3::gain_cal_enable.
/// \param gain_code 24-bit unsigned code; ::MCP3562_GAINCAL_UNITY (0x800000) corresponds to a 1x multiplier.
/// \return true on success.
bool mcp356x_set_gain_calibration(uint8_t device_address, uint32_t gain_code);

/// Reads back the GAINCAL code. \return true on success.
bool mcp356x_get_gain_calibration(uint8_t device_address, uint32_t *gain_code);

/// Locks the register map, blocking further Write commands until unlocked.
/// \details Writes LOCK[7:0] = ::MCP3562_LOCK_WRITE_DISABLE_CODE. Once locked, only the LOCK register itself remains 
// writable.
/// \return true on success.
bool mcp356x_lock(uint8_t device_address);

/// Unlocks the register map, restoring write access to the full register map.
/// \details Writes LOCK[7:0] = ::MCP3562_LOCK_WRITE_ENABLE_CODE.
/// \return true on success.
bool mcp356x_unlock(uint8_t device_address);

/// Reads the CRC-16 register-map checksum (CRCCFG), continuously updated while the device is locked. \return true on 
// success.
bool mcp356x_get_crc(uint8_t device_address, uint16_t *crc);

/// Identifies the connected device variant by reading the RESERVED register at address 0xE.
/// \details Also serves as a device-presence check: a failed transfer or an ::MCP3562_DEVICE_UNKNOWN
/// result indicates the device is not responding or not an MCP3561/2/4.
/// \return true if the SPI transaction completed (regardless of whether the variant was recognized).
bool mcp356x_identify(uint8_t device_address, mcp356x_device_variant_t *variant);

/// \}

/// \defgroup mcp356x_conversion ADC Conversion Helpers.
/// Reading and interpreting ADCDATA.
/// \{

/// Checks whether a new ADC conversion result is ready, via the cheap STATUS-byte poll.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \return true if ADCDATA has been updated since the last read.
bool mcp356x_is_data_ready(uint8_t device_address);

/// Reads and decodes the latest ADC conversion result from ADCDATA.
/// \param device_address 2-bit SPI device address of the target MCP356x.
/// \param format Data format currently configured via CONFIG3::data_format; determines how many bytes
/// are read (3 for ::MCP356x_DATA_FORMAT_24BIT, 4 otherwise) and how the result is decoded.
/// \param channel_id Optional; when @p format is ::MCP356x_DATA_FORMAT_32BIT_CHID_SGN, receives the
/// 4-bit SCAN Channel ID (see Table 5-14) that produced the result. Pass NULL if not needed, or when
/// using any other format (in which case it is left untouched).
/// \return the raw code conversion result.
uint32_t mcp356x_read_adc(uint8_t device_address, mcp356x_data_format_t format, uint8_t *channel_id);

/// Converts a signed ADCDATA code into a voltage, per Equation 5-5 (rearranged for VIN).
/// \details `VIN = (code / 8388608) * (vref / gain)`, where 8,388,608 = 2^23 is the ideal LSb weight
/// of the 23-bit-plus-sign coding. Does not account for offset/gain error unless @p code already has
/// digital calibration applied (i.e. EN_OFFCAL/EN_GAINCAL were enabled in CONFIG3 at conversion time).
/// \param code Signed conversion result, as returned by mcp356x_read_adc().
/// \param vref Reference voltage in volts (REFIN+ - REFIN-).
/// \param gain Front-end gain that was active when @p code was acquired.
/// \return The corresponding input voltage in volts.
float mcp356x_convert_to_voltage(int32_t code, float vref, mcp356x_gain_t gain);

/// \}

#ifdef __cplusplus
}
#endif

#endif /* MCP356X_API_H */
