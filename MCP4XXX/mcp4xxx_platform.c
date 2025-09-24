/**
 * @file mcp4xxx_platform.c
 * @brief Implementation of platform-specific I2C functions for MCP4XXX devices
 * 
 * This file implements the hardware abstraction layer for I2C communication
 * with MCP4XXX digital potentiometer devices. These functions should be 
 * implemented according to the specific platform being used.
 * 
 * @author Alejandro Beltran
 * @date 2025-05
 */

#include "mcp4xxx_platform.h"
#include <stdbool.h>

#include "peripheral/sercom/i2c_master/plib_sercom4_i2c_master.h"

bool mcp4xxx_i2c_write(uint8_t device_address, uint16_t data)
{
	// TODO - Implement the I2C write function (2 bytes) for the specific platform
    return false;
}

uint16_t mcp4xxx_i2c_read(uint8_t device_address, uint8_t read_command)
{
	// TODO - Implement the I2C read function for the specific platform
    // - write the command to the device
    // - Read the 2-byte response from the device (MSB first)
	return 0xFFFF;
}

bool mcp4xxx_i2c_write_byte(uint8_t device_address, uint8_t data){
    // TODO - Implement the I2C write byte function for the specific platform
    return false;
}