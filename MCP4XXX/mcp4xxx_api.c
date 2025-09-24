/**
 * @file mcp4xxx_api.c
 * @brief API implementation for MCP4XXX digital potentiometer
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#include "mcp4xxx_api.h"
#include "mcp4xxx_platform.h"

bool mcp4xxx_check(uint8_t device_address)
{
    // Check device presence by reading the STATUS register
    uint16_t tcon = mcpxxx_read(device_address, TCON_ADDRESS);
    if (tcon != 0x1FF)
    {
        return false; // Device not responding
    }
    return true; // Device initialized successfully
}

bool mcpxxx_write(uint8_t device_address, uint8_t reg_address, uint16_t data)
{
    if (data > 0x1FF) // Check if data exceeds 9 bits
    {
        return false; // Invalid data for MCP4XXX
    }

    uint16_t command = (reg_address << 4) | WRITE_CMD | ((data & 0x1FF) >> 8);
    command <<= 8;
    command |= (data & 0xFF);
    return mcp4xxx_i2c_write(device_address, command);
}

uint16_t mcpxxx_read(uint8_t device_address, uint8_t reg_address)
{
    uint8_t command = (reg_address << 4) | READ_CMD;
    return mcp4xxx_i2c_read(device_address, command);
}

bool mcp4xxx_set_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value)
{
    uint8_t reg_address = (uint8_t)wiper;
    return mcpxxx_write(device_address, reg_address, value);
}

uint16_t mcp4xxx_get_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
{
    uint8_t reg_address = (uint8_t)wiper;
    return mcpxxx_read(device_address, reg_address);
}

bool mcp4xxx_increment_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
{
    uint8_t command = ((uint8_t)wiper << 4) | INCREMENT_CMD;
    return mcp4xxx_i2c_write_byte(device_address, command);
}

bool mcp4xxx_decrement_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
{
    uint8_t command = ((uint8_t)wiper << 4) | DECREMENT_CMD;
    return mcp4xxx_i2c_write_byte(device_address, command);
}

bool mcp4xxx_set_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper, uint16_t value)
{
    uint8_t reg_address = (wiper == WIPER_0) ? NV_WIPER_0_ADDRESS : NV_WIPER_1_ADDRESS;
    mcpxxx_write(device_address, reg_address, value);
    return true; // Assume write is successful, as MCP4XXX does not provide a direct confirmation
}

uint16_t mcp4xxx_get_nv_wiper(uint8_t device_address, mcp4xxx_wiper_t wiper)
{
    uint8_t reg_address = (wiper == WIPER_0) ? NV_WIPER_0_ADDRESS : NV_WIPER_1_ADDRESS;
    return mcpxxx_read(device_address, reg_address);
}