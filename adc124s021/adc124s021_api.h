/**
 * @file adc124s021_api.h
 * @brief API for interacting with the ADC124S021 4-channel 12-bit ADC.
 * 
 * This header file provides the function declarations and data structures
 * for initializing and reading data from the ADC124S021. It abstracts the
 * SPI communication and provides a user-friendly interface for accessing
 * ADC values.
 * 
 * @author Alejandro Beltran
 * @date May 2025
 */

#ifndef ADC124S021_API_H
#define ADC124S021_API_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the ADC124S021.
 * 
 * This function sets up the SPI interface required to communicate with the ADC124S021.
 * It must be called before any other functions in this library.
 */
void adc124s021_init(void);

/**
 * @brief Reads the ADC value from the specified channel.
 * 
 * @param channel The ADC channel to read (0-3).
 * @return uint16_t The 12-bit ADC value from the specified channel. Returns 0xFFFF if the channel is invalid.
 */
uint16_t adc124s021_read_channel(uint8_t channel);

/**
 * @brief Struct to hold ADC values for all channels.
 * 
 * This struct contains an array of 4 elements, where each element corresponds to the ADC value
 * from one of the channels (0-3).
 */
typedef struct {
    uint16_t channel[4];    /**< ADC values for channels 0-3. */
    float voltage[4];       /**< Voltage values for channels 0-3. */
} adc124s021_data_t;

/**
 * @brief Reads the ADC values from all 4 channels.
 * 
 * This function reads the ADC values from all 4 channels (0-3) and returns them in an
 * `adc124s021_data_t` struct. The values are stored in the `channel` array of the struct.
 * 
 * @return adc124s021_data_t A struct containing the ADC values for all 4 channels.
 */
adc124s021_data_t adc124s021_read_all_channels(void);

/**
 * @brief Set the reference voltage for ADC calculations
 * 
 * The reference voltage is used to convert the digital readings
 * to voltage values. Valid range is 2.7V to 5.25V.
 *
 * @param vref Reference voltage in volts
 * @return bool true if successful, false if vref is out of valid range
 */
bool adc124s021_set_vref(float vref);

/**
 * @brief Get the current reference voltage setting
 * 
 * @return float Current reference voltage in volts
 */
float adc124s021_get_vref(void);

#ifdef __cplusplus
}
#endif

#endif // ADC124S021_API_H
