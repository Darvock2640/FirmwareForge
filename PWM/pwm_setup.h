/**
 * @file pwm_setup.h
 * @brief Header file that declare the PWM device instances and their functions.
 *
 * Use this file to declare as many PWM devices as needed.
 * 
 * @author Alejandro Beltran
 * @date Feb 2026
 */

#ifndef PWM_SETUP_H
#define PWM_SETUP_H

#include "pwm_api.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// Hanlde declarations for PWM device 1
extern pwm_dev_t pwm_device;
// Function declarations for PWM device 1
extern pwm_functions_t pwm_functions;
// Function prototypes for PWM device 1, These use the generic API function signatures. @see pwm_functions_t in 
// pwm_api.h for details on the expected behavior of each function.
pwm_status_t init (pwm_dev_t* device, pwm_config_t* config);
pwm_status_t set_clk_freq (pwm_dev_t* device, float freq_hz);
pwm_status_t set_pwm_freq (pwm_dev_t* device, float freq_hz);
pwm_status_t set_duty_cycle (pwm_dev_t* device, float duty_cycle);
pwm_status_t disable (pwm_dev_t* device);

//---------------------------------- Declarations for additional PWM devices -------------------------------------------
// // Handle declarations for PWM device n
// extern pwm_dev_t pwm_deviceN;
// // Function declarations for PWM device n
// extern pwm_functions_t pwm_functionsN;
// // Function prototypes for PWM device n, These use the generic API function signatures.
// pwm_status_t initN (pwm_dev_t* device, pwm_config_t* config);
// pwm_status_t set_clk_freqN (pwm_dev_t* device, float freq_hz);
// pwm_status_t set_pwm_freqN (pwm_dev_t* device, float freq_hz);
// pwm_status_t set_duty_cycleN (pwm_dev_t* device, float duty_cycle);
// pwm_status_t disableN (pwm_dev_t* device);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* PWM_SETUP_H */