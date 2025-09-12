/**
 * @file ads8866_platform.h
 * @brief Platform-specific interface for ADS8866 16-bit ADC
 * 
 * This file defines the hardware abstraction layer (HAL) interface that must be
 * implemented for each target platform to support the ADS8866 ADC.
 * 
 * The ADS8866 is a 16-bit, 100kSPS, single-channel SAR ADC with SPI interface.
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef ADS8866_PLATFORM_H
#define ADS8866_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read data from ADS8866 via SPI
 * 
 * This function performs SPI communication with the ADS8866 ADC to read
 * the conversion result. This function must implement a 2-byte read.
 * 
 * @return uint16_t The 16-bit conversion result from the ADC
 */
uint16_t ads8866_platform_spi_read(void);

#ifdef __cplusplus
}
#endif

#endif /* ADS8866_PLATFORM_H */
