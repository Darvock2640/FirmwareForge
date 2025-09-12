/**
 * @file adc124s021_api.c
 * @brief Implementation of the API for interacting with the ADC124S021.
 * 
 * This file contains the implementation of functions for initializing and
 * reading data from the ADC124S021. It uses platform-specific SPI functions
 * to communicate with the ADC.
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#include "adc124s021_api.h"
#include "adc124s021_platform.h"

static float adc124s021_vref = 3.3f; // Default reference voltage for the ADC124S021

/**
 * @brief Initializes the ADC124S021.
 * 
 * This function sets up the SPI interface required to communicate with the ADC124S021.
 */
void adc124s021_init(void) {
    adc124s021_read_channel(0); // Dummy read to initialize the ADC.
}

/**
 * @brief Reads the ADC value from the specified channel.
 * 
 * @param channel The ADC channel to read (0-3).
 * @return uint16_t The 12-bit ADC value from the specified channel. Returns 0xFFFF if the channel is invalid.
 */
uint16_t adc124s021_read_channel(uint8_t channel) {
    if (channel > 3) {
        return 0xFFFF; // Invalid channel, return 0xFFFF.
    }

    uint16_t command = (channel & 0x03) << 11; // Prepare the command for the channel.
    adc124s021_platform_spi_transfer(command); // Read the previous sent channel and set the current channel
    uint16_t response = adc124s021_platform_spi_transfer(0x0000); // Read the current channel value and set channel 0 for the next read.

    return response & 0x0FFF; // Extract the 12 least significant bits.
}

/**
 * @brief Reads the ADC values from all 4 channels.
 * 
 * This function reads the ADC values from all 4 channels (0-3) and returns them in an
 * `adc124s021_data_t` struct. The values are stored in the `channel` array of the struct.
 * 
 * @return adc124s021_data_t A struct containing the ADC values for all 4 channels.
 */
adc124s021_data_t adc124s021_read_all_channels(void) {
    adc124s021_data_t data = {0}; // Initialize the data struct to zero.
    uint16_t value = 0; // Variable to hold the ADC value.

    for (uint8_t i = 1; i < 4; i++) {
        value = adc124s021_platform_spi_transfer((uint16_t)i << 11) & 0x00FFF; // Read the previous sent channel and set the current channel
        data.channel[i-1] = value;
        data.voltage[i-1] = (value * adc124s021_vref) / 4096.0f; // Convert the ADC value to voltage.
    }
    value = adc124s021_platform_spi_transfer(0x0000) & 0x00FFF; // Read the current channel value and set channel 0 for the next read.
    data.channel[3] = value;
    data.voltage[3] = (value * adc124s021_vref) / 4096.0f; // Convert the ADC value to voltage.
    return data;
}

/**
 * @brief Set the reference voltage for ADC calculations
 * 
 * The reference voltage is used to convert the digital readings
 * to voltage values. Valid range is 2.7V to 5.25V.
 *
 * @param vref Reference voltage in volts
 * @return bool true if successful, false if vref is out of valid range
 */
bool adc124s021_set_vref(float vref){
    if (vref >= 2.7 && vref <= 5.25) {
        adc124s021_vref = vref;
        return true;
    }
    return false;
}

/**
 * @brief Get the current reference voltage setting
 * 
 * @return float Current reference voltage in volts
 */
float adc124s021_get_vref(void){
    return adc124s021_vref;
}
