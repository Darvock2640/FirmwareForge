/**
 * @file mcp48fvxx_api.c
 * @brief Implementation of API for controlling MCP48FVXX series DACs
 * @author Alejandro Beltran
 * @date 2025-05
 */

#include "mcp48fvxx_api.h"
#include "mcp48fvxx_platform.h"

/**
 * @brief Set the output value of a DAC channel
 *
 * This function sets the output value for the specified DAC channel.
 * The value must be in the range 0-4095 (12-bit).
 *
 * @param channel Channel selection (0 for Channel A, 1 for Channel B)
 * @param value 12-bit output value (0-4095)
 * @return bool true if successful, false if an error occurred
 */
bool mcp48fvxx_set_output(uint8_t channel, uint16_t value) {
    if(channel > 1) {
        mcp48fvxx_error_handler("Invalid channel specified. Use 0 for Channel A or 1 for Channel B.");
        return 0;
    }else if (value > 4095) {
        mcp48fvxx_error_handler("Invalid value specified. Value must be in the range 0-4095.");
        return 0;
    }
    uint32_t result = 0;
    uint32_t command = 0;
    // Construct the command word
    command |= (channel == MCP48FVXX_CHANNEL_B) ? MCP48FVXX_CHANNEL_B_ADDRESS : MCP48FVXX_CHANNEL_A_ADDRESS; // Select channel
    command |= value;
    result = mcp48fvxx_spi_transfer(command);
    if(!(result & 0x10000)) {
        mcp48fvxx_error_handler("Error in address + command combination.");
        return 0;
    }
    return 1;
}

/**
 * @brief Turn a DAC channel on or off
 *
 * This function controls the power state of the specified DAC channel.
 * When turned off, the channel enters power-down mode.
 *
 * @param channel Channel selection (0 for Channel A, 1 for Channel B)
 * @param on_off true to turn the channel on, false to turn it off
 * @return bool true if successful, false if an error occurred
 */
bool mcp48fvxx_channel_on_off(uint8_t channel, bool on_off){
    if(channel > 1) {
        mcp48fvxx_error_handler("Invalid channel specified. Use 0 for Channel A or 1 for Channel B.");
        return 0;
    }
    
    uint32_t result = 0;
    uint32_t command = 0;
    // Construct the command word
    command |= MCP48FVXX_ON_OFF_REG | MCP48FVXX_READ;
    result = mcp48fvxx_spi_transfer(command);
    
    if(!(result & 0x10000)) {
        mcp48fvxx_error_handler("Error in address + command combination.");
        return 0;
    }
    
    result &= 0x0F;
    result &= ~(0x03 << (channel * 2));
    result |= on_off ? MCP48FVXX_CHANNEL_ON << (channel * 2) : MCP48FVXX_CHANNEL_OFF << (channel * 2);
    command = 0;
    command |= MCP48FVXX_ON_OFF_REG | result; 
    result = mcp48fvxx_spi_transfer(command);
    
    if(!(result & 0x10000)) {
        mcp48fvxx_error_handler("Error in address + command combination.");
        return 0;
    }
    
    return 1;
}