/// \file mcp356x_api.c
/// API implementation for the Microchip MCP356x 24-bit Delta-Sigma ADC
/// \author Alejandro Beltran
/// \date September 2026

#include "mcp356x_api.h"
#include "mcp356x_platform.h"

#include <string.h>

/// Builds the 8-bit COMMAND byte: CMD[7:6] device address, CMD[5:2]
/// register/fast-command, CMD[1:0] command type.
static inline uint8_t mcp356x_build_command_byte(uint8_t device_address, uint8_t addr_or_cmd, mcp356x_cmd_type_t type) {
  return (uint8_t)(((device_address & 0x03U) << 6) | ((addr_or_cmd & 0x0FU) << 2) | ((uint8_t)type & 0x03U));
}

/// Decodes a raw STATUS byte.
static mcp356x_status_t mcp356x_decode_status_byte(uint8_t status_byte) {
  mcp356x_status_t status;

  status.device_addr_ack = (uint8_t)((status_byte >> 3) & 0x07U);
  status.data_ready = ((status_byte >> 2) & 0x01U) == 0U; ///< DR_STATUS: 0 = new data ready.
  status.crc_error = ((status_byte >> 1) & 0x01U) == 0U; ///< CRCCFG_STATUS: 0 = CRC error occurred.
  status.por_occurred = (status_byte & 0x01U) == 0U; ///< POR_STATUS: 0 = POR occurred.

  return status;
}

/// Sign-extends a 24-bit two's complement value held in the low 24 bits of \p
/// raw.
static inline int32_t mcp356x_sign_extend_24(uint32_t raw) {
  return (raw & 0x00800000UL) ? (int32_t)(raw | 0xFF000000UL) : (int32_t)raw;
}

bool mcp356x_write_register(uint8_t device_address, mcp356x_register_t reg, const uint8_t *data, uint8_t length, 
                            mcp356x_status_t *status) {
  uint8_t tx[4] = {0};
  uint8_t rx[4] = {0};

  if ((data == NULL) || (length == 0U) || (length > 3U)) {
    return false;
  }

  tx[0] = mcp356x_build_command_byte(device_address, (uint8_t)reg, 
                                MCP356X_CMD_TYPE_INCREMENTAL_WRITE);
  memcpy(&tx[1], data, length);

  if (!mcp356x_spi_transfer(tx, rx, (size_t)(length + 1U))) {
    return false;
  }

  if (status != NULL) {
    *status = mcp356x_decode_status_byte(rx[0]);
  }
  return true;
}

bool mcp356x_read_register(uint8_t device_address, mcp356x_register_t reg, uint8_t *data, uint8_t length, 
                           mcp356x_status_t *status) {
  uint8_t tx[5] = {0};
  uint8_t rx[5] = {0};

  if ((data == NULL) || (length == 0U) || (length > 4U)) {
    return false;
  }

  tx[0] = mcp356x_build_command_byte(device_address, (uint8_t)reg, MCP356X_CMD_TYPE_STATIC_READ);

  if (!mcp356x_spi_transfer(tx, rx, (size_t)(length + 1U))) {
    return false;
  }

  if (status != NULL) {
    *status = mcp356x_decode_status_byte(rx[0]);
  }

  memcpy(data, &rx[1], length);

  return true;
}

bool mcp356x_fast_command(uint8_t device_address, mcp356x_fast_cmd_t command, mcp356x_status_t *status) {
  uint8_t tx[1];
  uint8_t rx[1] = {0};

  tx[0] = mcp356x_build_command_byte(device_address, (uint8_t)command, MCP356X_CMD_TYPE_FAST);

  if (!mcp356x_spi_transfer(tx, rx, 1U)) {
    return false;
  }

  if (status != NULL) {
    *status = mcp356x_decode_status_byte(rx[0]);
  }

  return true;
}

bool mcp356x_get_status(uint8_t device_address, mcp356x_status_t *status) {
  uint8_t tx[1];
  uint8_t rx[1] = {0};

  if (status == NULL) {
    return false;
  }

  /* CMD[5:2] = 0x0 with CMD[1:0] = Fast is the "Don't Care" code (Table 6-2):
   * ignored by the device but still returns a fully latched STATUS byte,
   * without touching any register. */
  tx[0] = mcp356x_build_command_byte(device_address, 0x0U, MCP356X_CMD_TYPE_FAST);

  if (!mcp356x_spi_transfer(tx, rx, 1U)) {
    return false;
  }

  *status = mcp356x_decode_status_byte(rx[0]);

  return true;
}

bool mcp356x_reset(uint8_t device_address) {
  return mcp356x_fast_command(device_address, MCP356X_FASTCMD_FULL_RESET, NULL);
}

bool mcp356x_start_conversion(uint8_t device_address) {
  return mcp356x_fast_command(device_address, MCP356X_FASTCMD_ADC_START_RESTART, NULL);
}

bool mcp356x_standby(uint8_t device_address) {
  return mcp356x_fast_command(device_address, MCP356X_FASTCMD_ADC_STANDBY, NULL);
}

bool mcp356x_adc_shutdown(uint8_t device_address) {
  return mcp356x_fast_command(device_address, MCP356X_FASTCMD_ADC_SHUTDOWN, NULL);
}

bool mcp356x_full_shutdown(uint8_t device_address) {
  return mcp356x_fast_command(device_address, MCP356X_FASTCMD_FULL_SHUTDOWN, NULL);
}

bool mcp356x_set_config0(uint8_t device_address, const mcp356x_config0_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  /// CONFIG0[7:6] are forced to '11' (their POR value): per Register 8-2, they
  /// only have an effect (forcing Full Shutdown) when set to '00' together with
  /// every other CONFIG0 bit.
  value = (uint8_t)((0x3U << 6) | (((uint8_t)config->clk_sel & 0x3U) << 4) |
                    (((uint8_t)config->bias_current & 0x3U) << 2) |
                    ((uint8_t)config->adc_mode & 0x3U));

  return mcp356x_write_register(device_address, MCP356X_REG_CONFIG0, &value, 1U, NULL);
}

bool mcp356x_get_config0(uint8_t device_address, mcp356x_config0_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_CONFIG0, &value, 1U, NULL)) {
    return false;
  }

  config->clk_sel = (mcp356x_clk_sel_t)((value >> 4) & 0x3U);
  config->bias_current = (mcp356x_bias_current_t)((value >> 2) & 0x3U);
  config->adc_mode = (mcp356x_adc_mode_t)(value & 0x3U);

  return true;
}

bool mcp356x_set_config1(uint8_t device_address, const mcp356x_config1_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  value = (uint8_t)((((uint8_t)config->prescale & 0x3U) << 6) | (((uint8_t)config->osr & 0xFU) << 2));

  return mcp356x_write_register(device_address, MCP356X_REG_CONFIG1, &value, 1U, NULL);
}

bool mcp356x_get_config1(uint8_t device_address, mcp356x_config1_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_CONFIG1, &value, 1U, NULL)) {
    return false;
  }

  config->prescale = (mcp356x_prescale_t)((value >> 6) & 0x3U);
  config->osr = (mcp356x_osr_t)((value >> 2) & 0xFU);

  return true;
}

bool mcp356x_set_config2(uint8_t device_address, const mcp356x_config2_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  /* RESERVED[1:0] must always read/write as '11' (Register 8-4). */
  value = (uint8_t)((((uint8_t)config->boost & 0x3U) << 6) |
                    (((uint8_t)config->gain & 0x7U) << 3) |
                    ((config->az_mux_enable ? 1U : 0U) << 2) | 0x3U);

  return mcp356x_write_register(device_address, MCP356X_REG_CONFIG2, &value, 1U, NULL);
}

bool mcp356x_get_config2(uint8_t device_address, mcp356x_config2_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_CONFIG2, &value, 1U, NULL)) {
    return false;
  }

  config->boost = (mcp356x_boost_t)((value >> 6) & 0x3U);
  config->gain = (mcp356x_gain_t)((value >> 3) & 0x7U);
  config->az_mux_enable = ((value >> 2) & 0x1U) != 0U;

  return true;
}

bool mcp356x_set_config3(uint8_t device_address, const mcp356x_config3_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  value = (uint8_t)((((uint8_t)config->conv_mode & 0x3U) << 6) |
                    (((uint8_t)config->data_format & 0x3U) << 4) |
                    ((config->crc_format_32bit ? 1U : 0U) << 3) |
                    ((config->crc_com_enable ? 1U : 0U) << 2) |
                    ((config->offset_cal_enable ? 1U : 0U) << 1) |
                    (config->gain_cal_enable ? 1U : 0U));

  return mcp356x_write_register(device_address, MCP356X_REG_CONFIG3, &value, 1U, NULL);
}

bool mcp356x_get_config3(uint8_t device_address, mcp356x_config3_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_CONFIG3, &value, 1U, NULL)) {
    return false;
  }

  config->conv_mode = (mcp356x_conv_mode_t)((value >> 6) & 0x3U);
  config->data_format = (mcp356x_data_format_t)((value >> 4) & 0x3U);
  config->crc_format_32bit = ((value >> 3) & 0x1U) != 0U;
  config->crc_com_enable = ((value >> 2) & 0x1U) != 0U;
  config->offset_cal_enable = ((value >> 1) & 0x1U) != 0U;
  config->gain_cal_enable = (value & 0x1U) != 0U;

  return true;
}

bool mcp356x_set_irq(uint8_t device_address, const mcp356x_irq_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  value = (uint8_t)(((config->mdat_output_enable ? 1U : 0U) << 3) |
                    ((config->irq_inactive_high ? 1U : 0U) << 2) |
                    ((config->fast_cmd_enable ? 1U : 0U) << 1) |
                    (config->conv_start_irq_enable ? 1U : 0U));

  return mcp356x_write_register(device_address, MCP356X_REG_IRQ, &value, 1U, NULL);
}

bool mcp356x_get_irq(uint8_t device_address, mcp356x_irq_t *config) {
  uint8_t value;

  if (config == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_IRQ, &value, 1U, NULL)) {
    return false;
  }

  config->data_ready = ((value >> 6) & 0x1U) == 0U;
  config->crc_error = ((value >> 5) & 0x1U) == 0U;
  config->por_occurred = ((value >> 4) & 0x1U) == 0U;
  config->mdat_output_enable = ((value >> 3) & 0x1U) != 0U;
  config->irq_inactive_high = ((value >> 2) & 0x1U) != 0U;
  config->fast_cmd_enable = ((value >> 1) & 0x1U) != 0U;
  config->conv_start_irq_enable = (value & 0x1U) != 0U;

  return true;
}

bool mcp356x_set_mux(uint8_t device_address, mcp356x_mux_input_t vin_plus, mcp356x_mux_input_t vin_minus) {
  uint8_t value = (uint8_t)((((uint8_t)vin_plus & 0xFU) << 4) | ((uint8_t)vin_minus & 0xFU));
  return mcp356x_write_register(device_address, MCP356X_REG_MUX, &value, 1U, NULL);
}

bool mcp356x_get_mux(uint8_t device_address, mcp356x_mux_input_t *vin_plus, mcp356x_mux_input_t *vin_minus) {
  uint8_t value;

  if ((vin_plus == NULL) || (vin_minus == NULL)) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_MUX, &value, 1U, NULL)) {
    return false;
  }

  *vin_plus = (mcp356x_mux_input_t)((value >> 4) & 0xFU);
  *vin_minus = (mcp356x_mux_input_t)(value & 0xFU);

  return true;
}

bool mcp356x_set_scan(uint8_t device_address, uint16_t channel_mask, mcp356x_scan_delay_t delay) {
  uint32_t reg = (((uint32_t)delay & 0x7UL) << 21) | (uint32_t)channel_mask;
  uint8_t data[3];

  data[0] = (uint8_t)((reg >> 16) & 0xFFU);
  data[1] = (uint8_t)((reg >> 8) & 0xFFU);
  data[2] = (uint8_t)(reg & 0xFFU);

  return mcp356x_write_register(device_address, MCP356X_REG_SCAN, data, 3U, NULL);
}

bool mcp356x_get_scan(uint8_t device_address, uint16_t *channel_mask, mcp356x_scan_delay_t *delay) {
  uint8_t data[3];
  uint32_t reg;

  if ((channel_mask == NULL) || (delay == NULL)) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_SCAN, data, 3U, NULL)) {
    return false;
  }

  reg = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[2];

  *delay = (mcp356x_scan_delay_t)((reg >> 21) & 0x7UL);
  *channel_mask = (uint16_t)(reg & 0xFFFFUL);

  return true;
}

bool mcp356x_set_timer(uint8_t device_address, uint32_t delay_counts) {
  uint8_t data[3];

  delay_counts &= 0x00FFFFFFUL;
  data[0] = (uint8_t)((delay_counts >> 16) & 0xFFU);
  data[1] = (uint8_t)((delay_counts >> 8) & 0xFFU);
  data[2] = (uint8_t)(delay_counts & 0xFFU);

  return mcp356x_write_register(device_address, MCP356X_REG_TIMER, data, 3U, NULL);
}

bool mcp356x_get_timer(uint8_t device_address, uint32_t *delay_counts) {
  uint8_t data[3];

  if (delay_counts == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_TIMER, data, 3U, NULL)) {
    return false;
  }

  *delay_counts = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[2];

  return true;
}

bool mcp356x_set_offset_calibration(uint8_t device_address, int32_t offset_code) {
  uint32_t raw = (uint32_t)offset_code & 0x00FFFFFFUL;
  uint8_t data[3];

  data[0] = (uint8_t)((raw >> 16) & 0xFFU);
  data[1] = (uint8_t)((raw >> 8) & 0xFFU);
  data[2] = (uint8_t)(raw & 0xFFU);

  return mcp356x_write_register(device_address, MCP356X_REG_OFFSETCAL, data, 3U, NULL);
}

bool mcp356x_get_offset_calibration(uint8_t device_address, int32_t *offset_code) {
  uint8_t data[3];
  uint32_t raw;

  if (offset_code == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_OFFSETCAL, data, 3U, NULL)) {
    return false;
  }

  raw = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[2];
  *offset_code = mcp356x_sign_extend_24(raw);

  return true;
}

bool mcp356x_set_gain_calibration(uint8_t device_address, uint32_t gain_code) {
  uint8_t data[3];

  gain_code &= 0x00FFFFFFUL;
  data[0] = (uint8_t)((gain_code >> 16) & 0xFFU);
  data[1] = (uint8_t)((gain_code >> 8) & 0xFFU);
  data[2] = (uint8_t)(gain_code & 0xFFU);

  return mcp356x_write_register(device_address, MCP356X_REG_GAINCAL, data, 3U, NULL);
}

bool mcp356x_get_gain_calibration(uint8_t device_address, uint32_t *gain_code) {
  uint8_t data[3];

  if (gain_code == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_GAINCAL, data, 3U, NULL)) {
    return false;
  }

  *gain_code = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[2];

  return true;
}

bool mcp356x_lock(uint8_t device_address) {
  uint8_t value = (uint8_t)MCP356X_LOCK_WRITE_DISABLE_CODE;

  return mcp356x_write_register(device_address, MCP356X_REG_LOCK, &value, 1U, NULL);
}

bool mcp356x_unlock(uint8_t device_address) {
  uint8_t value = (uint8_t)MCP356X_LOCK_WRITE_ENABLE_CODE;

  return mcp356x_write_register(device_address, MCP356X_REG_LOCK, &value, 1U, NULL);
}

bool mcp356x_get_crc(uint8_t device_address, uint16_t *crc) {
  uint8_t data[2];

  if (crc == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_CRCCFG, data, 2U, NULL)) {
    return false;
  }

  *crc = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);

  return true;
}

bool mcp356x_identify(uint8_t device_address, mcp356x_device_variant_t *variant) {
  uint8_t data[2];
  uint16_t value;

  if (variant == NULL) {
    return false;
  }

  if (!mcp356x_read_register(device_address, MCP356X_REG_RESERVED_E, data, 2U, NULL)) {
    return false;
  }

  value = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);

  switch (value) {
	case 0x000CU:
		*variant = MCP356X_DEVICE_MCP3561;
		break;
	case 0x000DU:
		*variant = MCP356X_DEVICE_MCP3562;
		break;
	case 0x000FU:
		*variant = MCP356X_DEVICE_MCP3564;
		break;
	default:
		*variant = MCP356X_DEVICE_UNKNOWN;
		break;
  }

  return true;
}

bool mcp356x_is_data_ready(uint8_t device_address) {
  mcp356x_status_t status;

   if (!mcp356x_get_status(device_address, &status)) {
    return false;
  }

  return status.data_ready;
}

uint32_t mcp356x_read_adc(uint8_t device_address, mcp356x_data_format_t format, uint8_t *channel_id) {
  uint8_t length = (format == MCP356X_DATA_FORMAT_24BIT) ? 3U : 4U;
  uint8_t data[4] = {0};
  uint32_t raw = 0;
  uint8_t i;

  if (!mcp356x_read_register(device_address, MCP356X_REG_ADCDATA, data, length, NULL)) {
    return false;
  }

  for (i = 0U; i < length; i++) {
    raw = (raw << 8) | data[i];
  }

  switch (format) {
	case MCP356X_DATA_FORMAT_24BIT:
		return mcp356x_sign_extend_24(raw);
		break;

	case MCP356X_DATA_FORMAT_32BIT_LEFT_JUSTIFIED:
		/* 24-bit two's complement data left-justified with a 0x00 pad byte in the
		* LSBs. */
		return mcp356x_sign_extend_24(raw >> 8);
		break;

	case MCP356X_DATA_FORMAT_32BIT_SGN_EXTENDED:
		/* The device already repeats the sign bit across the full 8-bit extension
		* byte, so the raw 32-bit pattern is a valid two's complement value as-is.
		*/
		return (int32_t)raw;
		break;

	case MCP356X_DATA_FORMAT_32BIT_CHID_SGN:
	default:
		if (channel_id != NULL) {
			*channel_id = (uint8_t)((raw >> 28) & 0xFU);
		}
		/* Remaining 28 bits are a 4-bit sign extension followed by the 24-bit data.
		*/
		raw &= 0x0FFFFFFFUL;
		return (raw & 0x08000000UL) ? (int32_t)(raw | 0xF0000000UL) : (int32_t)raw;
		break;
  }
}

float mcp356x_convert_to_voltage(int32_t code, float vref, mcp356x_gain_t gain) {
  static const float gain_lut[8] = {1.0f / 3.0f, 1.0f,  2.0f,  4.0f, 8.0f, 16.0f, 32.0f,
									64.0f};
  float divisor = gain_lut[(uint8_t)gain & 0x7U];

  return ((float)code / 8388608.0f) * (vref / divisor);
}
