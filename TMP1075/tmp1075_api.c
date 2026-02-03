
/**
 * @file tmp1075_api.c
 * @brief 
 *
 * This file provides the API implementation for interfacing with the TMP1075 temperature sensor.
 * 
 * @author Alejandro Beltran
 * @date November 2025
 */

#include "tmp1075_api.h"
#include "tmp1075_platform.h"
#include <stdint.h>

bool tmp1075_write(uint8_t device_address, uint8_t register_address, uint16_t data){
    if(!(register_address <= 0x03 || register_address == TMP1075_REG_ID)){
        return false; // Invalid register address
    }
    return tmp1075_i2c_write(device_address, register_address << 16 | data);
}

uint16_t tmp1075_read(uint8_t device_address, uint8_t register_address){
    if(!(register_address <= 0x03 || register_address == TMP1075_REG_ID)){
        return 0xFFFF; // Invalid register address
    }
    if(tmp1075_i2c_write_byte(device_address, register_address)){
        return tmp1075_i2c_read(device_address);
    }
    return 0xFFFF; // Read failed
}

uint16_t tmp1075_id_read(uint8_t device_address){
    return tmp1075_read(device_address, TMP1075_REG_ID);
}

bool tmp1075_one_shot_conversion(uint8_t device_address){
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config |= (1 << 15); // Set the one-shot bit
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_conversion_rate(uint8_t device_address, tmp1075_conversion_rate rate){
    if(rate > MS_220){
        return false; // Invalid conversion rate
    }
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config &= ~(0b11 << 13); // Clear the conversion rate bits
    config |= (rate << 13);  // Set the new conversion rate
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_fault_queue(uint8_t device_address, tmp1075_consecutive_fault faults){
    if(faults > FAULT_4){
        return false; // Invalid fault queue
    }
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config &= ~(0b11 << 11); // Clear the fault queue bits
    config |= (faults << 11);  // Set the new fault queue
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_alert_polarity(uint8_t device_address, tmp1075_alert_polarity polarity){
    if(polarity > POLARITY_1){
        return false; // Invalid alert polarity
    }
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config &= ~(1 << 10); // Clear the alert polarity bit
    config |= (polarity << 10);  // Set the new alert polarity
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_alert_function(uint8_t device_address, tmp1075_alert_function function){
    if(function > INTERRUPT){
        return false; // Invalid alert function
    }
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config &= ~(1 << 9); // Clear the alert function bit
    config |= (function << 9);  // Set the new alert function
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_power_mode(uint8_t device_address, tmp1075_power_mode mode){
    if(mode > SHUTDOWN){
        return false; // Invalid power mode
    }
    uint16_t config = tmp1075_read(device_address, TMP1075_REG_CONFIGURATION);
    config &= ~(1 << 8); // Clear the power mode bit
    config |= (mode << 8);  // Set the new power mode
    return tmp1075_write(device_address, TMP1075_REG_CONFIGURATION, config);
}

bool tmp1075_set_temperature_low_limit(uint8_t device_address, float temp_low){
    if(temp_low < MIN_TEMPERATURE || temp_low > MAX_TEMPERATURE){
        return false; // Temperature out of range
    }
    int16_t temp_raw = (int16_t)(temp_low / LSB_VALUE_IN_CELSIUS); // Convert to raw value
    return tmp1075_write(device_address, TMP1075_REG_TEMP_LOW, temp_raw << 4);
}

bool tmp1075_set_temperature_high_limit(uint8_t device_address, float temp_high){
    if(temp_high < MIN_TEMPERATURE || temp_high > MAX_TEMPERATURE){
        return false; // Temperature out of range
    }
    int16_t temp_raw = (int16_t)(temp_high / LSB_VALUE_IN_CELSIUS); // Convert to raw value
    return tmp1075_write(device_address, TMP1075_REG_TEMP_HIGH, temp_raw << 4);
}

float tmp1075_read_temperature_low_limit(uint8_t device_address){
    uint16_t temp_raw = tmp1075_read(device_address, TMP1075_REG_TEMP_LOW);
    int16_t temp_signed = ((int16_t)(temp_raw)/SENSOR_VALUE_TO_DIGITAL_FACTOR); // Shift to get the signed value
    return temp_signed * LSB_VALUE_IN_CELSIUS;
}

float tmp1075_read_temperature_high_limit(uint8_t device_address){
    uint16_t temp_raw = tmp1075_read(device_address, TMP1075_REG_TEMP_HIGH);
    int16_t temp_signed = ((int16_t)(temp_raw)/SENSOR_VALUE_TO_DIGITAL_FACTOR); // Shift to get the signed value
    return temp_signed * LSB_VALUE_IN_CELSIUS;
}

float tmp1075_read_temperature(uint8_t device_address){
    uint16_t temp_raw = tmp1075_read(device_address, TMP1075_REG_TEMPERATURE);
    int16_t temp_signed = ((int16_t)(temp_raw)/SENSOR_VALUE_TO_DIGITAL_FACTOR); // Shift to get the signed value
    return temp_signed * LSB_VALUE_IN_CELSIUS;
}