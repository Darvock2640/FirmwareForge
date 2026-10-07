/// \file mcp356x_platform.h
/// Platform-specific SPI interface for the MCP356X ADC
/// 
/// \details Declares the hardware abstraction layer the mcp356x_api.c protocol
/// implementation calls into. Porting this library to a new microcontroller or SPI
/// peripheral only requires re-implementing mcp356x_platform.c; mcp356x_api.h/.c never
/// need to change.
/// 
/// \author Alejandro Beltran
/// \date September 2026

#ifndef MCP356X_PLATFORM_H
#define MCP356X_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Performs one full-duplex SPI transaction with the MCP356x.
/// \details Exchanges \p length bytes, transmitting \p tx_buffer on SDI while simultaneously
/// capturing SDO into \p rx_buffer. The first byte of \p tx_buffer is always the COMMAND byte,
/// and the first byte captured into \p rx_buffer is always the STATUS byte.
/// This function must NOT drive CS: the library supports multiple devices on the same bus, so the
/// application asserts the CS of the target device before each API call and deasserts it after.
/// The transfer must be blocking (all bytes exchanged before returning), because the application
/// deasserts CS as soon as the API call returns.
/// \param tx_buffer Bytes to transmit on SDI, COMMAND byte first. Must not be NULL.
/// \param rx_buffer Buffer to receive bytes clocked out on SDO, STATUS byte first. Must not be NULL.
/// \param length Number of bytes to exchange (COMMAND byte plus any data bytes).
/// \return true if the transfer completed successfully, false otherwise.
bool mcp356x_spi_transfer(const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* MCP356X_PLATFORM_H */
