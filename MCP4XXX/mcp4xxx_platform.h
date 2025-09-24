/**
 * @file mcp4xxx_platform.h
 * @brief Platform-specific I2C interface for MCP4XXX digital potentiometer devices
 * 
 * This file declares the hardware abstraction layer functions needed
 * to communicate with MCP4XXX digital potentiometer devices over I2C.
 * 
 * @author Alejandro Beltran
 * @date May - 2025
 */

#ifndef MCP4XXX_PLATFORM_H
#define MCP4XXX_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes data to an MCP4XXX device over I2C
 * 
 * @param device_address The 7-bit I2C address of the target device
 * @param data 16-bit data to be written to the device
 * @return bool true if write was successful, false otherwise
 */
bool mcp4xxx_i2c_write(uint8_t device_address, uint16_t data);

/**
 * @brief Reads data from an MCP4XXX device over I2C
 * 
 * @param device_address The 7-bit I2C address of the target device
 * @param read_command The command byte to initiate the read operation
 * @return uint16_t The 16-bit data read from the device, 0xFFFF on failure
 */
uint16_t mcp4xxx_i2c_read(uint8_t device_address, uint8_t read_command);

/**
 * @brief Write a byte to an MCP4XXX device over I2C
 * 
 * @param device_address The 7-bit I2C address of the target device
 * @param data The byte to be written to the device
 * @return bool true if write was successful, false otherwise
 */
bool mcp4xxx_i2c_write_byte(uint8_t device_address, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* MCP4XXX_PLATFORM_H */