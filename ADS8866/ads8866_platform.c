/**
 * @file ads8866_platform.c
 * @brief Implementation of platform-specific functions for ADS8866 ADC
 *
 * This file should implements the hardware abstraction layer (HAL) for
 * the ADS8866 ADC.
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#include "ads8866_platform.h"
#include <stdint.h>

#include "peripheral/sercom/spi_master/plib_sercom0_spi_master.h"

/**
 * @brief Read a 16-bit value from the ADS8866 ADC via SPI
 *
 * This function should be implemented to perform a 2-byte SPI read.
 *
 * @return uint16_t 16-bit conversion result
 */
uint16_t ads8866_platform_spi_read(void){
    // TODO: Implement SPI data transfer for the target platform.
    return 0;
}