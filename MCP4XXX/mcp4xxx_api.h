/**
 * @file mcp4xxx_api.h
 * @brief API for controlling MCP4XXX series digital potentiometers
 *
 * This library provides functions to interface with Microchip's MCP4XXX family
 * of digital potentiometers/rheostats over I2C. Supports both single and dual
 * channel devices with different resolution steps (129 or 257 steps).
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef MCP4XXX_API_H
#define MCP4XXX_API_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Command codes for MCP4XXX operations
 */
#define WRITE_CMD       0X00 << 2  /**< Command to write data to a register */
#define INCREMENT_CMD   0X01 << 2  /**< Command to increment wiper value */
#define DECREMENT_CMD   0X02 << 2  /**< Command to decrement wiper value */
#define READ_CMD        0X03 << 2  /**< Command to read data from a register */

/**
 * @brief MCP4XXX register addresses
 */
#define WIPER_0_ADDRESS     0x00  /**< Volatile wiper 0 register address */
#define WIPER_1_ADDRESS     0x01  /**< Volatile wiper 1 register address (dual devices only) */
#define NV_WIPER_0_ADDRESS  0x02  /**< Non-volatile wiper 0 register address */
#define NV_WIPER_1_ADDRESS  0x03  /**< Non-volatile wiper 1 register address (dual devices only) */
#define TCON_ADDRESS        0x04  /**< Terminal control register address */
#define STATUS_ADDRESS      0x05  /**< Status register address */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wiper selection for MCP4XXX devices
 * 
 * This enum defines the wipers that can be controlled on MCP4XXX digital potentiometers.
 * The available values are:
 * 
 * - WIPER_0 (0): Available on all MCP4XXX devices
 * 
 * - WIPER_1 (1): Available only on dual channel devices (MCP4641, MCP4642, MCP4661, MCP4662)
 */
typedef enum mcp4xxx_wiper {
    WIPER_0 = 0,      /**< Wiper 0 (available on all devices) */
    WIPER_1           /**< Wiper 1 (available only on dual channel devices) */
} mcp4xxx_wiper_t;

/**
 * @brief Check if an MCP4XXX device is present and functioning
 *
 * This function attempts to communicate with the device to verify its presence.
 *
 * @param device_address The I2C device address for the target device
 * @return true if device responds correctly, false otherwise
 */
bool mcp4xxx_check(uint8_t device_address);

/**
 * @brief Write data to a specified register on an MCP4XXX device
 *
 * @param device_address The I2C device address for the target device
 * @param reg_address Register address to write
 * @param data 16-bit data to write to the register (9 bits)
 * @return true if write was successful, false otherwise
 */
bool mcpxxx_write(uint8_t device_address, uint8_t reg_address, uint16_t data);

/**
 * @brief Read data from a specified register on an MCP4XXX device
 *
 * @param device_address The I2C device address for the target device
 * @param reg_address Register address to read from
 * @return 16-bit data read from the register
 */
uint16_t mcpxxx_read(uint8_t device_address, uint8_t reg_address);

/**
 * @brief Set the wiper position to a specific value
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to set (WIPER_0 or WIPER_1 for dual devices)
 * @param value The wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)
 * @return true if the wiper was set successfully, false otherwise
 */
bool mcp4xxx_set_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value);

/**
 * @brief Get the current wiper position
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to read (WIPER_0 or WIPER_1 for dual devices)
 * @return Current wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)
 */
uint16_t mcp4xxx_get_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper);

/**
 * @brief Increment the wiper position by one step
 *
 * This function increases the wiper position by one step. If the wiper is already
 * at the maximum position, it typically remains unchanged.
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to increment (WIPER_0 or WIPER_1 for dual devices)
 * @return true if the operation was successful, false otherwise
 */
bool mcp4xxx_increment_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper);

/**
 * @brief Decrement the wiper position by one step
 *
 * This function decreases the wiper position by one step. If the wiper is already
 * at the minimum position (0), it typically remains unchanged.
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to decrement (WIPER_0 or WIPER_1 for dual devices)
 * @return true if the operation was successful, false otherwise
 */
bool mcp4xxx_decrement_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper);

/**
 * @brief Set the non-volatile wiper position to a specific value
 *
 * This function writes to the non-volatile memory in the device.
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to set (WIPER_0 or WIPER_1 for dual devices)
 * @param value The wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)
 * @return true if the operation was successful, false otherwise
 */
bool mcp4xxx_set_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value);

/**
 * @brief Get the non-volatile wiper position
 *
 * @param device_address The I2C device address for the target device
 * @param wiper Which wiper to read (WIPER_0 or WIPER_1 for dual devices)
 * @return Non-volatile wiper position value (0-256 for 257-step devices, 0-128 for 129-step devices)
 */
uint16_t mcp4xxx_get_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper);

#ifdef __cplusplus
}
#endif

#endif /* MCP4XXX_API_H */
