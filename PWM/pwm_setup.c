/**
 * @file pwm_setup.c
 * @brief This file contains the implementation of the PWM device instances and their functions.
 *
 * This file should implement all the functions and instances declared in pwm_setup.h. The functions should have the 
 * hardware-specific code to control each of the PWM devices.
 * 
 * @author Alejandro Beltran
 * @date Feb 2026
 */

#include "pwm_setup.h"

#include "peripheral/tcc/plib_tcc0.h"
#include "peripheral/tcc/plib_tcc1.h"

// Functions initialization for PWM 1
pwm_functions_t pwm_functions = {
	.init = init,
	.set_clk_freq = set_clk_freq,
	.set_pwm_freq = set_pwm_freq,
	.set_duty_cycle = set_duty_cycle,
	.disable = disable
};

// Initialization of PWM 1 handle
pwm_dev_t pwm_device = {
	.functions = &pwm_functions,
	.config = {0},
	.hw_base = NULL
};

// functions implementation for PWM 1
pwm_status_t init(pwm_dev_t *dev, pwm_config_t *config) {
	if (dev && config) {
		// Suggested validation checks for a general purpose PWM configuration parameters. You can modify these as 
		// needed for your specific hardware capabilities.
		if (config->clk_frequency_hz > 0 &&
			config->pwm_frequency_hz > 0 &&
			config->duty_cycle_percent >= 0 &&
			config->duty_cycle_percent <= 100 &&
			config->pwm_frequency_hz * 2 <= config->clk_frequency_hz
			) {
			// Store the configuration in the device instance. This allows the API functions to access the current 
			// configuration.
			dev->config = *config;
			
			// TODO : Add hardware-specific initialization code here.
			return PWM_NOT_IMPLEMENTED;

			// After hardware initialization, set the clock frequency. This will also set the initial PWM frequency and 
			// duty cycle based on the provided configuration.
			pwm_status_t status;
			status = dev->functions->set_clk_freq(dev, config->clk_frequency_hz);
			if (status != PWM_OK) {
				return status;
			}
			return PWM_OK;
		}
	}
	return PWM_INVALID_PARAM;
}

pwm_status_t set_clk_freq(pwm_dev_t* device, float freq_hz){
	if (device && freq_hz > 0) {
		// Store the configuration in the device instance. This allows the API functions to access the current 
		// configuration.
		device->config.clk_frequency_hz = freq_hz;

		pwm_status_t status;
		// Suggested validation check: If the new clock frequency cannot support the currently configured PWM frequency,
		// you may want to adjust the PWM frequency or duty cycle to prevent hardware issues. This is just an example of
		// how you could handle such a case.
		if (freq_hz >= (2 * device->config.pwm_frequency_hz)) {
			// If the new clock can support the current PWM frequency, just update the PWM frequency to apply the new 
			// clock.
			status = device->functions->set_pwm_freq(device, device->config.pwm_frequency_hz);
		}
		else {
			// If the new clock cannot support the current PWM frequency, you could set the PWM frequency to the maximum
			// supported by the new clock (half of the clock frequency) and set the duty cycle to 0 to prevent any 
			// potential issues with unsupported configurations. This is just one way to handle this situation, and you 
			// may choose a different approach based on your specific requirements.
			status = device->functions->set_pwm_freq(device, freq_hz / 2);
			status = device->functions->set_duty_cycle(device, 0);
		}
		if (status != PWM_OK) {
			return status;
		}
		return PWM_OK;
	}
	return PWM_INVALID_PARAM;
}

pwm_status_t set_pwm_freq(pwm_dev_t* device, float freq_hz){
	if (device && freq_hz > 0 && freq_hz * 2 <= device->config.clk_frequency_hz) {
		// TODO
		// Suggested implementation for a general-purpose PWM device without preescaler
		uint32_t digital_period = (uint32_t)(device->config.clk_frequency_hz / freq_hz);
		// Here you would add hardware-specific code to set the pwm frequency.

		// Update the device configuration with the new frequency. This allows the API to keep track of the current 
		// settings.
		device->config.pwm_frequency_hz = freq_hz;
		return PWM_NOT_IMPLEMENTED;
		
		pwm_status_t status;
		// After changing the frequency, you may need to update the duty cycle to maintain the same percentage.
		status = device->functions->set_duty_cycle(device, device->config.duty_cycle_percent);
		if (status != PWM_OK) {
			return status;
		}
		return PWM_OK;
	}
	return PWM_INVALID_PARAM;
}

pwm_status_t set_duty_cycle(pwm_dev_t* device, float duty_cycle){
	if (device && duty_cycle >= 0 && duty_cycle <= 100) {
		// TODO
		// Suggested implementation for a general-purpose PWM device without preescaler.
		uint32_t digital_period = (uint32_t)(device->config.clk_frequency_hz / device->config.pwm_frequency_hz);
		uint32_t digital_duty = (uint32_t)((duty_cycle / 100.0f) * digital_period);
		// Here you would add hardware-specific code to set the pwm duty cycle.

		// Update the device configuration with the new duty cycle. This allows the API to keep track of the current 
		// settings.
		device->config.duty_cycle_percent = duty_cycle;
		return PWM_NOT_IMPLEMENTED;
	}
	return PWM_INVALID_PARAM;
}

pwm_status_t disable(pwm_dev_t* device) {
	if (device) {
		pwm_status_t status;
		status = device->functions->set_duty_cycle(device, 0.0f);
		if (status != PWM_OK) {
			return status;
		}
		return PWM_OK;
	}
	return PWM_INVALID_PARAM;
}

//----------------------------------- Implementations for PWM n --------------------------------------------------------
// // Functions initialization for PWM n
// pwm_functions_t pwm_functionsN = {
// 	.init = initN,
// 	.set_clk_freq = set_clk_freqN,
// 	.set_pwm_freq = set_pwm_freqN,
// 	.set_duty_cycle = set_duty_cycleN,
// 	.disable = disableN
// };

// // Initialization of PWM n handle
// pwm_dev_t pwm_deviceN = {
// 	.functions = &pwm_functionsN,
// 	.config = {0},
// 	.hw_base = NULL
// };

// // functions implementation for PWM n
// pwm_status_t initN(pwm_dev_t *dev, pwm_config_t *config) {
// 	if (dev && config) {
// 		// Suggested validation checks for a general purpose PWM configuration parameters. You can modify these as 
// 		// needed for your specific hardware capabilities.
// 		if (config->clk_frequency_hz > 0 &&
// 			config->pwm_frequency_hz > 0 &&
// 			config->duty_cycle_percent >= 0 &&
// 			config->duty_cycle_percent <= 100 &&
// 			config->pwm_frequency_hz * 2 <= config->clk_frequency_hz
// 			) {
// 			// Store the configuration in the device instance. This allows the API functions to access the current 
// 			// configuration.
// 			dev->config = *config;
			
// 			// TODO : Add hardware-specific initialization code here.
// 			return PWM_NOT_IMPLEMENTED;

// 			// After hardware initialization, set the clock frequency. This will also set the initial PWM frequency and 
// 			// duty cycle based on the provided configuration.
// 			pwm_status_t status;
// 			status = dev->functions->set_clk_fre(dev, config->clk_frequency_hz);
// 			if (status != PWM_OK) {
// 				return status;
// 			}
// 			return PWM_OK;
// 		}
// 	}
// 	return PWM_INVALID_PARAM;
// }

// pwm_status_t set_clk_freqN(pwm_dev_t* device, float freq_hz){
// 	if (device && freq_hz > 0) {
// 		// Store the configuration in the device instance. This allows the API functions to access the current 
// 		// configuration.
// 		device->config.clk_frequency_hz = freq_hz;

// 		pwm_status_t status;
// 		// Suggested validation check: If the new clock frequency cannot support the currently configured PWM frequency,
// 		// you may want to adjust the PWM frequency or duty cycle to prevent hardware issues. This is just an example of
// 		// how you could handle such a case.
// 		if (freq_hz >= (2 * device->config.pwm_frequency_hz)) {
// 			// If the new clock can support the current PWM frequency, just update the PWM frequency to apply the new 
// 			// clock.
// 			status = device->functions->set_pwm_freq(device, device->config.pwm_frequency_hz);
// 		}
// 		else {
// 			// If the new clock cannot support the current PWM frequency, you could set the PWM frequency to the maximum
// 			// supported by the new clock (half of the clock frequency) and set the duty cycle to 0 to prevent any 
// 			// potential issues with unsupported configurations. This is just one way to handle this situation, and you 
// 			// may choose a different approach based on your specific requirements.
// 			status = device->functions->set_pwm_freq(device, freq_hz / 2);
// 			status = device->functions->set_duty_cycle(device, 0);
// 		}
// 		if (status != PWM_OK) {
// 			return status;
// 		}
// 		return PWM_OK;
// 	}
// 	return PWM_INVALID_PARAM;
// }

// pwm_status_t set_pwm_freqN(pwm_dev_t* device, float freq_hz){
// 	if (device && freq_hz > 0 && freq_hz * 2 <= device->config.clk_frequency_hz) {
// 		// TODO
// 		// Suggested implementation for a general-purpose PWM device without preescaler
// 		uint32_t digital_period = (uint32_t)(device->config.clk_frequency_hz / freq_hz);
// 		// Here you would add hardware-specific code to set the pwm frequency.

// 		// Update the device configuration with the new frequency. This allows the API to keep track of the current 
// 		// settings.
// 		device->config.pwm_frequency_hz = freq_hz;
// 		return PWM_NOT_IMPLEMENTED;
		
// 		pwm_status_t status;
// 		// After changing the frequency, you may need to update the duty cycle to maintain the same percentage.
// 		status = device->functions->set_duty_cycle(device, device->config.duty_cycle_percent);
// 		if (status != PWM_OK) {
// 			return status;
// 		}
// 		return PWM_OK;
// 	}
// 	return PWM_INVALID_PARAM;
// }

// pwm_status_t set_duty_cycleN(pwm_dev_t* device, float duty_cycle){
// 	if (device && duty_cycle >= 0 && duty_cycle <= 100) {
// 		// TODO
// 		// Suggested implementation for a general-purpose PWM device without preescaler.
// 		uint32_t digital_period = (uint32_t)(device->config.clk_frequency_hz / device->config.pwm_frequency_hz);
// 		uint32_t digital_duty = (uint32_t)((duty_cycle / 100.0f) * digital_period);
// 		// Here you would add hardware-specific code to set the pwm duty cycle.

// 		// Update the device configuration with the new duty cycle. This allows the API to keep track of the current 
// 		// settings.
// 		device->config.duty_cycle_percent = duty_cycle;
// 		return PWM_NOT_IMPLEMENTED;
// 	}
// 	return PWM_INVALID_PARAM;
// }

// pwm_status_t disableN(pwm_dev_t* device) {
// 	if (device) {
// 		pwm_status_t status;
// 		status = device->functions->set_duty_cycle(device, 0.0f);
// 		if (status != PWM_OK) {
// 			return status;
// 		}
// 		return PWM_OK;
// 	}
// 	return PWM_INVALID_PARAM;
// }