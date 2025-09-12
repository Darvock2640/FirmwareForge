/**
 * @file adc124s021_platform.h
 * @brief Platform-specific SPI interface for the ADC124S021.
 * 
 * This header file declares the platform-specific functions required for
 * SPI communication with the ADC124S021. These functions must be implemented
 * for the specific hardware platform being used.
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef ADC124S021_PLATFORM_H
#define ADC124S021_PLATFORM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transfers data over SPI and returns the received data.
 * 
 * @param data The 16-bit data to send over SPI.
 * @return uint16_t The 16-bit data received from SPI.
 * 
 * This function sends a 16-bit command or data to the ADC124S021 over SPI and
 * simultaneously receives 16-bit data from the ADC. It must be implemented for
 * the specific platform.
 */
uint16_t adc124s021_platform_spi_transfer(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif // ADC124S021_PLATFORM_H
