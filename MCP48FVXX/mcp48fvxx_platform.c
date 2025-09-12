/**
 * @file mcp48fvxx_platform.c
 * @brief Platform-specific implementations for MCP48FVXX DAC
 * @author Alejandro Beltran
 * @date May 2025
 */

#include "mcp48fvxx_platform.h"

#include "peripheral/sercom/spi_master/plib_sercom0_spi_master.h"

uint32_t mcp48fvxx_spi_transfer(uint32_t command_24bit) {
    // TODO: Implement SPI 3 bytes data transfer for the target platform.
    return 0;    
}

void mcp48fvxx_error_handler(char *message){
    // TODO: Implement for the target platform.
    return;
}