/**
 * @file tmp1075_platform.h
 * @brief Platform-specific I2C interface for TMP1075 temperature sensor devices
 * 
 * This file declares the hardware abstraction layer functions needed
 * to communicate with TMP1075 temperature sensor devices over I2C.
 * 
 * @author Alejandro Beltran
 * @date November 2025
 */

#ifndef TMP1075_PLATFORM_H
#define TMP1075_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes data to the TMP1075 device over I2C.
 * 
 * This function sends a sequence of bytes to the specified device address.
 * Typically used to write to a register, where the data includes the register address
 * and the value to be written.
 * 
 * @param device_address The 7-bit I2C address of the TMP1075 device.
 * @param data The data to write. The format depends on the specific operation, 
 *             usually containing the register address and the data payload.
 * @return true if the write operation was successful, false otherwise.
 */
bool tmp1075_i2c_write(uint8_t device_address, uint32_t data);

/**
 * @brief Writes a single byte to the TMP1075 device over I2C.
 * 
 * This function is commonly used to set the register pointer before a read operation.
 * 
 * @param device_address The 7-bit I2C address of the TMP1075 device.
 * @param data The byte to write (e.g., register address).
 * @return true if the write operation was successful, false otherwise.
 */
bool tmp1075_i2c_write_byte(uint8_t device_address, uint8_t data);

/**
 * @brief Reads a 16-bit value from the TMP1075 device over I2C.
 * 
 * This function reads two bytes from the device and combines them into a 16-bit value.
 * It assumes the register pointer has already been set (e.g., by tmp1075_i2c_write_byte).
 * 
 * @param device_address The 7-bit I2C address of the TMP1075 device.
 * @return The 16-bit value read from the device. Returns 0xFFFF on failure (though 0xFFFF is also a valid value).
 */
uint16_t tmp1075_i2c_read(uint8_t device_address);

#ifdef __cplusplus
}
#endif

#endif /* TMP1075_PLATFORM_H */