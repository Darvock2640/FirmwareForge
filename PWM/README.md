# PWM API Documentation

## Overview

This PWM API provides a generic, hardware-independent API for creating and controlling instances of PWM (Pulse Width Modulation) devices. It defines a standard interface that allows users to interact with multiple PWM instances uniformly, regardless of the underlying hardware implementation (whether it be a dedicated hardware peripheral or a bit-banged solution).

## Hardware Information

This library acts as an abstraction layer. It does not interface directly with specific hardware but rather relies on platform-specific implementations that adhere to the defined `pwm_functions_t` interface. This design ensures that the application code remains portable and decoupled from the specific hardware details.

The API supports configuration of:
- Source clock frequency
- PWM output frequency
- Duty cycle (0.0% to 100.0%)

## Library Architecture

The library is organized into the following files:

1. **pwm_api.h**: Main API header file. It defines the device structures, configuration structs, return codes, and the inline API functions used by the application to control the PWM devices.
2. **pwm_setup.h**: Example header file for declaring specific PWM device instances and their associated platform-specific function prototypes.
3. **pwm_setup.c**: Example implementation file where PWM device instances are instantiated and linked to their specific driver implementations.

This separation allows for a clean distinction between the generic control logic and the specific hardware drivers.

## API Reference

### Data Types

#### `pwm_status_t`
An enumeration defining the return status codes for PWM operations.

| Value | Description |
|-------|-------------|
| `PWM_OK` | Operation successful |
| `PWM_ERROR` | General error occurred |
| `PWM_INVALID_PARAM` | Invalid parameter provided |
| `PWM_NOT_IMPLEMENTED` | Function not implemented for the specific device |

#### `pwm_config_t`
Structure holding the configuration parameters for a PWM signal.

| Field | Type | Description |
|-------|------|-------------|
| `clk_frequency_hz` | `float` | Source clock frequency in Hz |
| `pwm_frequency_hz` | `float` | Desired PWM output frequency in Hz |
| `duty_cycle_percent` | `float` | Desired duty cycle (0.0 - 100.0) |

#### `pwm_dev_t`
Handle to a PWM device instance. Contains pointers to the implementation functions, current configuration, and a pointer to the hardware base address.

### Functions

The following inline functions are provided in `pwm_api.h` to interact with PWM devices:

#### pwm_init

```c
pwm_status_t pwm_init(pwm_dev_t *dev, pwm_config_t *config)
```
Initializes a PWM device instance with the given configuration.
* `parameter` - `dev` Pointer to the PWM device instance.
* `parameter` - `config` Pointer to the configuration structure.
* `return` - `pwm_status_t` PWM_OK if successful, PWM_INVALID_PARAM if parameters are NULL, or driver specific error code.

#### pwm_set_clock
```c
pwm_status_t pwm_set_clock(pwm_dev_t* dev, float clk)
```
Sets the source clock frequency for the PWM device.

* `param` -  `dev` Pointer to the PWM device instance.
* `param` -  `clk` The source clock frequency in Hertz.
* `return` -  `pwm_status_t` PWM_OK if successful, or error code.

#### pwm_set_freq
```c
pwm_status_t pwm_set_freq(pwm_dev_t* dev, float freq_hz)
```
Sets the output frequency of the PWM signal.
* `param` - `dev` Pointer to the PWM device instance.
* `param` - `freq_hz` The desired PWM frequency in Hertz.
* `return` - `pwm_status_t` PWM_OK if successful, or error code.

#### pwm_set_duty_cycle
```c
pwm_status_t pwm_set_duty_cycle(pwm_dev_t* dev, float duty_cycle)
```
Sets the duty cycle of the PWM signal.
* `param` - `dev` Pointer to the PWM device instance.
* `param` - `duty_cycle` The desired duty cycle percentage (0.0 to 100.0).
* `return` - `pwm_status_t` PWM_OK if successful, or error code.

#### pwm_disable
```c
pwm_status_t pwm_disable(pwm_dev_t* dev)
```
Disables the PWM output.
* `param` - `dev` Pointer to the PWM device instance.
* `return` - `pwm_status_t` PWM_OK if successful, or error code.

#### pwm_get_clock
```c
float pwm_get_clock(pwm_dev_t* dev)
```
Gets the configured source clock frequency.
* `param` - `dev` Pointer to the PWM device instance.
* `return` - `float` The clock frequency in Hertz, or 0 if dev is NULL.

#### pwm_get_freq
```c
float pwm_get_freq(pwm_dev_t* dev)
```
Gets the configured PWM output frequency.
* `param` - `dev` Pointer to the PWM device instance.
* `return` - `float` The PWM frequency in Hertz, or 0 if dev is NULL.

#### pwm_get_duty_cycle
```c
float pwm_get_duty_cycle(pwm_dev_t* dev)
```
Gets the configured duty cycle.
* `param` - `dev` Pointer to the PWM device instance.
* `return` - `float` The duty cycle percentage, or 0 if dev is NULL.

## Usage Examples
Basic PWM initialization and control.
### main.c
```c
#include "../PWM/pwm_setup.h"

pwm_config_t pwm_config = {
    .clk_frequency_hz = 48000000.0f,
    .pwm_frequency_hz = 20000.0f,
    .duty_cycle_percent = 50.0f
};
pwm_dev_t* pwm = &pwm_device;

int main ( void )
{
    pwm_init(pwm, &pwm_config);

    while ( true )
    {
        pwm_set_freq(pwm, 10000.0f);
        pwm_set_duty_cycle(pwm, 75.0f);
    }
}
```

### pwm_setup.h
```c
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

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* PWM_SETUP_H */
```

### pwm_setup.c
Example platform implementation for Microchip ***SAMD51*** with [MPLABX MCC HArmony](https://github.com/Microchip-MPLAB-Harmony) <br>
```c
#include "pwm_setup.h"
#include "peripheral/tcc/plib_tcc0.h"

// Functions initialization for PWM
pwm_functions_t pwm_functions = {
	.init = init,
	.set_clk_freq = set_clk_freq,
	.set_pwm_freq = set_pwm_freq,
	.set_duty_cycle = set_duty_cycle,
	.disable = disable
};

// Initialization of PWM
pwm_dev_t pwm_device = {
	.functions = &pwm_functions,
	.config = {0},
	.hw_base = NULL
};

// functions implementation for PWM
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
			TCC0_PWMStart();

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
		TCC0_PWM24bitPeriodSet(digital_period);
		// Update the device configuration with the new frequency. This allows the API to keep track of the current 
		// settings.
		device->config.pwm_frequency_hz = freq_hz;
		// return PWM_NOT_IMPLEMENTED;
		
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
		TCC0_PWM24bitDutySet(TCC0_CHANNEL0, digital_duty);

		// Update the device configuration with the new duty cycle. This allows the API to keep track of the current 
		// settings.
		device->config.duty_cycle_percent = duty_cycle;
		// return PWM_NOT_IMPLEMENTED;
		return PWM_OK;
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
```