/**
 * @file tmp1075_api.h
 * @brief 
 * 
 * This file contains the API for interfacing with the TMP1075 temperature sensor.
 * 
 * @author Alejandro Beltran
 * @date November 2025
 */

#ifndef TMP1075_API_H
#define TMP1075_API_H

#include "stdint.h"
#include "stdbool.h"
#include <stdint.h>

/**
 * @brief Factor to convert sensor value to digital value.
 * 
 * The temperature register is a 12-bit register, left-justified in a 16-bit word.
 * The 4 LSBs are 0.
 */
#define SENSOR_VALUE_TO_DIGITAL_FACTOR  16

/**
 * @brief Least Significant Bit value in Celsius.
 * 
 * The resolution of the TMP1075 is 0.0625 degrees Celsius.
 */
#define LSB_VALUE_IN_CELSIUS            0.0625f

/**
 * @brief Minimum measurable temperature in Celsius.
 */
#define MIN_TEMPERATURE                 -55.0f

/**
 * @brief Maximum measurable temperature in Celsius.
 */
#define MAX_TEMPERATURE                 150.0f

#define TMP1075_REG_TEMPERATURE         0x00  /**< Temperature Register (Read Only) */
#define TMP1075_REG_CONFIGURATION       0x01  /**< Configuration Register (Read/Write) */
#define TMP1075_REG_TEMP_LOW            0x02  /**< Temperature Low Limit Register (Read/Write) */
#define TMP1075_REG_TEMP_HIGH           0x03  /**< Temperature High Limit Register (Read/Write) */
#define TMP1075_REG_ID                  0x0F  /**< Device ID Register (Read Only) */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Conversion rate settings for the TMP1075.
 * 
 * These values correspond to the R1 and R0 bits in the Configuration Register.
 * Options:
 * - MS_27_5: 27.5 ms conversion rate
 * - MS_55: 55 ms conversion rate
 * - MS_110: 110 ms conversion rate
 * - MS_220: 220 ms conversion rate
 */
typedef enum tmp1075_convertion_rate{
    MS_27_5 = 0b00,     /**< 27.5 ms conversion rate */
    MS_55   = 0b01,     /**< 55 ms conversion rate */
    MS_110  = 0b10,     /**< 110 ms conversion rate */
    MS_220  = 0b11      /**< 220 ms conversion rate */
} tmp1075_conversion_rate;

/**
 * @brief Fault queue settings.
 * 
 * These values correspond to the F1 and F0 bits in the Configuration Register.
 * The fault queue defines the number of consecutive faults required to trigger the alert.
 * Options:
 * - FAULT_1: 1 Consecutive fault measurement
 * - FAULT_2: 2 Consecutive fault measurements
 * - FAULT_3: 3 Consecutive fault measurements
 * - FAULT_4: 4 Consecutive fault measurements
 */
typedef enum tmp1075_consecutive_fault{
    FAULT_1 = 0b00,     /**< 1 Consecutive fault measurement to trigger the alert function */
    FAULT_2 = 0b01,     /**< 2 Consecutive fault measurements to trigger the alert function */
    FAULT_3 = 0b10,     /**< 3 Consecutive fault measurements to trigger the alert function */
    FAULT_4 = 0b11      /**< 4 Consecutive fault measurements to trigger the alert function */
} tmp1075_consecutive_fault;

/**
 * @brief Alert pin polarity settings.
 * 
 * These values correspond to the POL bit in the Configuration Register.
 * Options:
 * - POLARITY_0: Active low ALERT pin
 * - POLARITY_1: Active high ALERT pin
 */
typedef enum tmp1075_alert_polarity{
    POLARITY_0 = 0,     /**< Active low ALERT pin */
    POLARITY_1 = 1,     /**< Active high ALERT pin */
} tmp1075_alert_polarity;
    
/**
 * @brief Alert function mode settings.
 * 
 * These values correspond to the TM bit in the Configuration Register.
 * Options:
 * - COMPARATOR: Comparator mode
 * - INTERRUPT: Interrupt mode
 */
typedef enum tmp1075_alert_function{
    COMPARATOR = 0,     /**< Comparator mode */
    INTERRUPT  = 1      /**< Interrupt mode */
} tmp1075_alert_function;

/**
 * @brief Power mode settings.
 * 
 * These values correspond to the SD bit in the Configuration Register.
 * Options:
 * - CONTINUOUS: Continuous conversion mode
 * - SHUTDOWN: Shutdown mode
 */
typedef enum tmp1075_power_mode{
    CONTINUOUS = 0,     /**< Continuous conversion mode */
    SHUTDOWN   = 1      /**< Shutdown mode */
} tmp1075_power_mode;

/**
 * @brief Writes a 16-bit value to a specific register of the TMP1075.
 * 
 * @param device_address The I2C address of the device.
 * @param register_address The address of the register to write to.
 * @param data The 16-bit data to write.
 * @return true if the write operation was successful, false otherwise.
 */
bool tmp1075_write(uint8_t device_address, uint8_t register_address, uint16_t data);

/**
 * @brief Reads a 16-bit value from a specific register of the TMP1075.
 * 
 * @param device_address The I2C address of the device.
 * @param register_address The address of the register to read from.
 * @return The 16-bit value read from the register, or 0xFFFF if the read failed or address is invalid.
 */
uint16_t tmp1075_read(uint8_t device_address, uint8_t register_address);

/**
 * @brief Reads the device ID of the TMP1075.
 * 
 * @param device_address The I2C address of the device.
 * @return The 16-bit device ID.
 */
uint16_t tmp1075_id_read(uint8_t device_address);

/**
 * @brief Triggers a one-shot conversion.
 * 
 * @param device_address The I2C address of the device.
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_one_shot_conversion(uint8_t device_address);

/**
 * @brief Sets the conversion rate of the TMP1075.
 * 
 * @param device_address The I2C address of the device.
 * @param rate The desired conversion rate (see tmp1075_convertion_rate).
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_conversion_rate(uint8_t device_address, tmp1075_conversion_rate rate);

/**
 * @brief Sets the number of consecutive faults required to trigger an alert.
 * 
 * @param device_address The I2C address of the device.
 * @param faults The number of consecutive faults (see tmp1075_consecutive_fault).
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_fault_queue(uint8_t device_address, tmp1075_consecutive_fault faults);

/**
 * @brief Sets the polarity of the ALERT pin.
 * 
 * @param device_address The I2C address of the device.
 * @param polarity The desired polarity (see tmp1075_alert_polarity).
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_alert_polarity(uint8_t device_address, tmp1075_alert_polarity polarity);

/**
 * @brief Sets the mode of the ALERT pin (Comparator or Interrupt).
 * 
 * @param device_address The I2C address of the device.
 * @param function The desired alert function (see tmp1075_alert_function).
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_alert_function(uint8_t device_address, tmp1075_alert_function function);

/**
 * @brief Sets the power mode of the device (Continuous or Shutdown).
 * 
 * @param device_address The I2C address of the device.
 * @param mode The desired power mode (see tmp1075_power_mode).
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_power_mode(uint8_t device_address, tmp1075_power_mode mode);

/**
 * @brief Sets the low temperature limit.
 * 
 * @param device_address The I2C address of the device.
 * @param temp_low The low temperature limit in Celsius.
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_temperature_low_limit(uint8_t device_address, float temp_low);

/**
 * @brief Sets the high temperature limit.
 * 
 * @param device_address The I2C address of the device.
 * @param temp_high The high temperature limit in Celsius.
 * @return true if the operation was successful, false otherwise.
 */
bool tmp1075_set_temperature_high_limit(uint8_t device_address, float temp_high);

/**
 * @brief Reads the low temperature limit.
 * 
 * @param device_address The I2C address of the device.
 * @return The low temperature limit in Celsius.
 */
float tmp1075_read_temperature_low_limit(uint8_t device_address);

/**
 * @brief Reads the high temperature limit.
 * 
 * @param device_address The I2C address of the device.
 * @return The high temperature limit in Celsius.
 */
float tmp1075_read_temperature_high_limit(uint8_t device_address);

/**
 * @brief Reads the current temperature from the device.
 * 
 * @param device_address The I2C address of the device.
 * @return The current temperature in Celsius.
 */
float tmp1075_read_temperature(uint8_t device_address);

#ifdef __cplusplus
}
#endif

#endif // TMP1075_API_H
