/**
 * @file mcp48fvxx_api.h
 * @brief API for controlling MCP48FVXX series DACs
 * 
 * This file provides functions and definitions for controlling 
 * MCP48FVXX series Digital-to-Analog Converters.
 *
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef MCP48FVXX_API_H
#define MCP48FVXX_API_H

#include "mcp48fvxx_platform.h"             // Include platform-specific functions

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// MCP48FVXX configuration constants
/** @brief Channel A selection */
#define MCP48FVXX_CHANNEL_A 0x00
/** @brief Channel B selection */
#define MCP48FVXX_CHANNEL_B 0x01
/** @brief Channel A register address */
#define MCP48FVXX_CHANNEL_A_ADDRESS 0x00
/** @brief Channel B register address */
#define MCP48FVXX_CHANNEL_B_ADDRESS 0x01 << 19
/** @brief Power control register address */
#define MCP48FVXX_ON_OFF_REG 0x09 << 19
/** @brief Command to write to the DAC */
#define MCP48FVXX_WRITE 0b00
/** @brief Command to read from the DAC */
#define MCP48FVXX_READ 0b11 << 17
/** @brief Channel on (normal operation) */
#define MCP48FVXX_CHANNEL_ON 0x00
/** @brief Channel off (power down) */
#define MCP48FVXX_CHANNEL_OFF 0x01

/**
 * @brief Set the output value of a DAC channel
 * 
 * @param channel Channel selection (0 for Channel A, 1 for Channel B)
 * @param value 12-bit output value (0-4095)
 * @return bool true if successful, false if an error occurred
 */
bool mcp48fvxx_set_output(uint8_t channel, uint16_t value);

/**
 * @brief Turn a DAC channel on or off
 * 
 * @param channel Channel selection (0 for Channel A, 1 for Channel B)
 * @param on_off true to turn the channel on, false to turn it off
 * @return bool true if successful, false if an error occurred
 */
bool mcp48fvxx_channel_on_off(uint8_t channel, bool on_off);

#ifdef __cplusplus
}
#endif

#endif // MCP48FVXX_API_H
