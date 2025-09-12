/**
 * @file mcp48fvxx_platform.h
 * @brief Platform-specific functions for MCP48FVXX DAC
 * 
 * This file defines platform-specific functions that must be implemented
 * for each target platform to support the MCP48FVXX DAC.
 *
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef MCP48FVXX_PLATFORM_H
#define MCP48FVXX_PLATFORM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform SPI transfer with the DAC
 * 
 * This function handles the SPI communication protocol for the MCP48FVXX DAC.
 * It sends a 24-bit command and returns the 24-bit response.
 * 
 * @param command_24bit 24-bit command to send to the DAC
 * @return uint32_t 24-bit response from the DAC
 */
uint32_t mcp48fvxx_spi_transfer(uint32_t command_24bit);

/**
 * @brief Handle error messages from the DAC library
 * 
 * This function should implement appropriate error handling
 * for the target platform (e.g., logging, display, etc.).
 * 
 * @param message Error message string
 */
void mcp48fvxx_error_handler(char *message);

#ifdef __cplusplus
}
#endif

#endif // MCP48FVXX_PLATFORM_H
