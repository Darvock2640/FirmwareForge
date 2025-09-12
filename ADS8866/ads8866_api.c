/**
 * @file ads8866_api.c
 * @brief Implementation of the ADS8866 ADC API functions
 *
 * This file implements the functions for interfacing with the ADS8866 ADC,
 * including reading conversions and voltage calculations.
 *
 * @author Alejandro Beltran
 * @date May 2025
 */

#include "ads8866_api.h"

/**
 * @brief Default reference voltage for the ADC
 * 
 * The ADS8866 can operate with reference voltages between 2.5V and 5.0V.
 * Default is set to 3.3V (typical supply voltage).
 */
static float ads8866_vref = 3.3f;

/**
 * @brief Read a conversion from the ADS8866 ADC
 * 
 * This function performs the following steps:
 * 1. Read the raw 16-bit value from the ADC via SPI
 * 2. Calculate the corresponding voltage using the formula:
 *    voltage = (digital_value * vref) / 65536
 * 
 * @return ads8866_data_t Structure with digital value and calculated voltage
 */
ads8866_data_t ads8866_read(void){
    ads8866_data_t data = {0};              // Initialize data structure
    uint16_t result = 0;

    // Perform SPI read
    result = ads8866_platform_spi_read();

    data.digital_value = result;
    data.voltage = (result * ads8866_vref) / 65536.0f; 

    return data;
}

/**
 * @brief Set the ADC reference voltage for voltage calculations
 * 
 * Validates that the provided reference voltage is within the allowed
 * range for the ADS8866 (2.5V to 5.0V) before setting it.
 *
 * @param vref Reference voltage in volts
 * @return bool true if successful, false if vref is out of valid range
 */
bool ads8866_set_vref(float vref){
    if (vref >= 2.5 && vref <= 5.0) {
        ads8866_vref = vref;
        return true;
    }
    return false;
}

/**
 * @brief Get the current reference voltage setting
 * 
 * @return float Current reference voltage in volts
 */
float ads8866_get_vref(void){
    return ads8866_vref;
}