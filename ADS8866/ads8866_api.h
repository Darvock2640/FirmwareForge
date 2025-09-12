/**
 * @file ads8866_api.h
 * @brief Application Programming Interface for ADS8866 16-bit ADC
 *
 * This file provides an API for interfacing with the ADS8866 ADC.
 * The ADS8866 is a 16-bit, single-channel, 100 kSPS, SAR analog-to-digital converter
 * with an SPI-compatible serial interface.
 *
 * Key features:
 * - 16-bit resolution
 * - 16 MHz SPI interface
 * - 2.7V to 3.6V analog supply range
 * - 1.65V to 3.6V digital supply range
 * - 2.5V to 5.0V reference voltage range
 *
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef ADS8866_API_H
#define ADS8866_API_H

#include <stdint.h>
#include "ads8866_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data structure for ADS8866 conversion results
 * 
 * This structure holds both the raw digital value read from the ADC
 * and the corresponding calculated voltage.
 */
typedef struct {
    uint16_t digital_value;  /**< Digital value read from ADC (0-65535) */
    float voltage;           /**< Voltage value calculated from digital value (in volts) */
} ads8866_data_t; 

/**
 * @brief Read a conversion result from the ADS8866 ADC
 * 
 * This function reads a conversion from the ADC via SPI interface
 * and calculates the corresponding voltage based on the reference voltage.
 *
 * @return ads8866_data_t Structure containing both digital and analog values
 */
ads8866_data_t ads8866_read(void);

/**
 * @brief Set the reference voltage for ADC calculations
 * 
 * The reference voltage is used to convert the digital readings
 * to voltage values. Valid range is 2.5V to 5.0V for the ADS8866.
 *
 * @param vref Reference voltage in volts
 * @return bool true if successful, false if vref is out of valid range
 */
bool ads8866_set_vref(float vref);

/**
 * @brief Get the current reference voltage setting
 * 
 * @return float Current reference voltage in volts
 */
float ads8866_get_vref(void);

#ifdef __cplusplus
}
#endif

#endif /* ADS8866_API_H */
