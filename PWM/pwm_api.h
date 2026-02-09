/**
 * @file pwm_api.h
 * @brief API for creating and controlling instances of generic PWM devices. 
 *
 * This API defines a class-type PWM device. This API allows users to create multiple instances of PWM devices and 
 * unify the functions to control any of them. The API is hardware independent, and each instance can have its own 
 * implementation (multiple peripherals or bit banging).
 * 
 * @author Alejandro Beltran
 * @date Feb 2026
 */
 
#ifndef PWM_API_H
#define PWM_API_H

#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

/**
 * @brief PWM return status codes
 * 
 * This enum defines the PWM status return codes used by the PWM API functions.
 *
 * - PWM_OK: The operation was successful.
 * - PWM_ERROR: A general error occurred during the operation.
 * - PWM_INVALID_PARAM: An invalid parameter was provided to the function.
 * - PWM_NOT_IMPLEMENTED: The function is not implemented for the specific PWM device.
 *
 */
typedef enum pwm_status_t {
    PWM_OK = 0,
    PWM_ERROR = -1,
    PWM_INVALID_PARAM = -2,
    PWM_NOT_IMPLEMENTED = -3,
} pwm_status_t; 

/**
 * @brief Handle to a PWM device instance.
*/
typedef struct pwm_dev_t pwm_dev_t;

/**
 * @brief Configuration structure for PWM settings.
 *
 * This structure holds the necessary parameters to configure a PWM signal, including clock source frequency, target 
 * output frequency, and the duty cycle.
 */
typedef struct pwm_config_t {
    /** The frequency of the source clock driving the PWM peripheral in Hertz (Hz). */
    float    clk_frequency_hz;
    /** The desired frequency of the PWM output signal in Hertz (Hz). */
    float    pwm_frequency_hz;
    /** The desired duty cycle of the PWM signal as a percentage (0.0 to 100.0). */
    float    duty_cycle_percent;
} pwm_config_t;


/**
 * @brief Structure containing function pointers for PWM operations.
 *
 * This structure defines the standard interface for interacting with a PWM device driver. It provides methods for 
 * initialization, frequency configuration, duty cycle control, and disabling the device.
 */
typedef struct pwm_functions_t {
    /**
     * @brief Initializes the PWM device with the specified configuration.
     *
     * @param device Pointer to the PWM device instance.
     * @param config Pointer to the configuration structure containing initial settings.
     * @return pwm_status_t Status of the initialization (e.g., PWM_OK, PWM_ERROR).
     */
    pwm_status_t (*init)(pwm_dev_t *device, pwm_config_t *config);

    /**
     * @brief Sets the source clock frequency for the PWM peripheral.
     *
     * @param device Pointer to the PWM device instance.
     * @param freq_hz The desired clock frequency in Hertz.
     * @return pwm_status_t Status of the operation.
     */
    pwm_status_t (*set_clk_freq)(pwm_dev_t *device, float freq_hz);

    /**
     * @brief Sets the frequency of the PWM signal.
     *
     * @param device Pointer to the PWM device instance.
     * @param freq_hz The desired PWM signal frequency in Hertz.
     * @return pwm_status_t Status of the operation.
     */
    pwm_status_t (*set_pwm_freq)(pwm_dev_t *device, float freq_hz);

    /**
     * @brief Sets the duty cycle of the PWM signal.
     *
     * @param device Pointer to the PWM device instance.
     * @param duty_cycle The desired duty cycle percentage as a float (0.00 to 100.00)
     * @return pwm_status_t Status of the operation.
     */
    pwm_status_t (*set_duty_cycle)(pwm_dev_t *device, float duty_cycle);

    /**
     * @brief Sets the duty cycle to 0 to stop the signal. To re-enable, set a new duty cycle.
     *
     * @param device Pointer to the PWM device instance.
     * @return pwm_status_t Status of the operation.
     */
    pwm_status_t (*disable)(pwm_dev_t *device);
} pwm_functions_t;

/**
 * @brief PWM device structure.
 *
 * This structure represents a PWM device instance. It encapsulates the low-level driver functions, hardware 
 * configuration, and the specific hardware register base address required to operate a specific PWM channel or 
 * peripheral.
 */
typedef struct pwm_dev_t {
    /** @see pwm_functions_t */
    const pwm_functions_t *functions;   /* Function pointers (in Flash) */
    
    /** @see pwm_config_t */
    pwm_config_t config;               /* Hardware specific data (Register base, channel ID) */
    
    /** Generic pointer to the base directory of the hardware registers for this specific PWM peripheral instance. */
    void* hw_base;
} pwm_dev_t;

/**
 * @brief Initializes a PWM device instance.
 *
 * This function initializes the PWM device using the provided configuration. It calls the driver-specific
 * initialization function.
 *
 * @param dev Pointer to the PWM device instance.
 * @param config Pointer to the configuration structure.
 * @return pwm_status_t PWM_OK if successful, PWM_INVALID_PARAM if parameters are NULL, or driver specific error code.
 */
static inline pwm_status_t pwm_init(pwm_dev_t *dev, pwm_config_t *config) {
    if (!dev || !config) {
		return PWM_INVALID_PARAM;
    }
    if (dev->functions->init) {
        pwm_status_t status = dev->functions->init(dev, config);
        if (status == PWM_OK) {
            dev->config = *config;
        }
        return status;
    }
    return PWM_NOT_IMPLEMENTED;
}

/**
 * @brief Sets the source clock frequency for the PWM device.
 *
 * @param dev Pointer to the PWM device instance.
 * @param clk The source clock frequency in Hertz.
 * @return pwm_status_t PWM_OK if successful, or error code.
 */
static inline pwm_status_t pwm_set_clock(pwm_dev_t* dev, float clk) {
    if (!dev) {
		return PWM_INVALID_PARAM;
    }
    if (dev->functions->set_clk_freq) {
		pwm_status_t status;
		status = dev->functions->set_clk_freq(dev, clk);
        if (status == PWM_OK) {
			dev->config.clk_frequency_hz = clk;
        }
		return status;
    }
    return PWM_NOT_IMPLEMENTED;
}

/**
 * @brief Sets the output frequency of the PWM signal.
 *
 * @param dev Pointer to the PWM device instance.
 * @param freq_hz The desired PWM frequency in Hertz.
 * @return pwm_status_t PWM_OK if successful, or error code.
 */
static inline pwm_status_t pwm_set_freq(pwm_dev_t* dev, float freq_hz) {
    if (!dev) {
        return PWM_INVALID_PARAM;
    }
    if (dev->functions->set_pwm_freq) {
        pwm_status_t status;
        status = dev->functions->set_pwm_freq(dev, freq_hz);
        if (status == PWM_OK) {
            dev->config.pwm_frequency_hz = freq_hz;
        }
        return status;
    }
    return PWM_NOT_IMPLEMENTED;
}

/**
 * @brief Sets the duty cycle of the PWM signal.
 *
 * @param dev Pointer to the PWM device instance.
 * @param duty_cycle The desired duty cycle percentage (0.0 to 100.0).
 * @return pwm_status_t PWM_OK if successful, or error code.
 */
static inline pwm_status_t pwm_set_duty_cycle(pwm_dev_t* dev, float duty_cycle) {
    if (!dev) {
        return PWM_INVALID_PARAM;
    }
    if (dev->functions->set_duty_cycle) {
        pwm_status_t status;
        status = dev->functions->set_duty_cycle(dev, duty_cycle);
        if (status == PWM_OK) {
            dev->config.duty_cycle_percent = duty_cycle;
        }
        return status;
    }
    return PWM_NOT_IMPLEMENTED;
}

/**
 * @brief Disables the PWM output.
 *
 * @param dev Pointer to the PWM device instance.
 * @return pwm_status_t PWM_OK if successful, or error code.
 */
static inline pwm_status_t pwm_disable(pwm_dev_t* dev) {
    if (dev && dev->functions->disable) {
        return dev->functions->disable(dev);
    }
    return PWM_INVALID_PARAM;
}

/**
 * @brief Gets the configured source clock frequency.
 *
 * @param dev Pointer to the PWM device instance.
 * @return float The clock frequency in Hertz, or 0 if dev is NULL.
 */
static inline float pwm_get_clock(pwm_dev_t* dev) {
    if (dev) {
        return dev->config.clk_frequency_hz;
    }
    return 0;
}

/**
 * @brief Gets the configured PWM output frequency.
 *
 * @param dev Pointer to the PWM device instance.
 * @return float The PWM frequency in Hertz, or 0 if dev is NULL.
 */
static inline float pwm_get_freq(pwm_dev_t* dev) {
    if (dev) {
        return dev->config.pwm_frequency_hz;
    }
    return 0;
}

/**
 * @brief Gets the configured duty cycle.
 *
 * @param dev Pointer to the PWM device instance.
 * @return float The duty cycle percentage, or 0 if dev is NULL.
 */
static inline float pwm_get_duty_cycle(pwm_dev_t* dev) {
    if (dev) {
        return dev->config.duty_cycle_percent;
    }
    return 0;
}

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // PWM_API_H