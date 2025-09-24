# FirmwareForge

## Table of Contents
- [FirmwareForge](#firmwareforge)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Features](#features)
  - [Library Structure](#library-structure)
    - [Available Libraries](#available-libraries)
  - [Getting Started](#getting-started)
  - [Usage](#usage)
  - [Contributing](#contributing)
  - [License](#license)

## About
This repository contains a collection of public firmware libraries designed for various embedded systems and microcontrollers. The goal is to provide a set of reusable, well-documented, and modular components to accelerate firmware development for hobbyists and professionals alike.

The libraries are written in C and are intended to be hardware-agnostic where possible, with clear separation of hardware-specific drivers and generic logic.

## Features
- Modular Design: Each library is self-contained and can be used independently.

- Clear Documentation: Code is heavily commented, and each library includes a detailed description in its respective directory.

- Hardware Abstraction: Aims to separate hardware-specific code from core functionality, making the libraries more portable.

- Open Source: Licensed under the MIT License, allowing for flexible use and modification.

## Library Structure

The repository is organized into subfolders, each containing a standalone library for a specific device or protocol. Each library includes its own source files and documentation.

### Available Libraries

- **[ADS8866](ADS8866/)**: 16-bit SPI ADC library for Texas Instruments ADS8866.
- **[MCP48FVXX](MCP48FVXX/)**: 12-bit dual-channel SPI DAC library for Microchip MCP48FVXX series.
- **[adc124s021](adc124s021/)**: 4-channel, 12-bit SPI ADC library for Texas Instruments ADC124S021.
- **[CDC_Console_USB](CDC_Console_USB/)**: USB CDC Example of use for Microchip 32 bits microcontrollers using MPLAB Harmony (MCC).
- **[MCP4XXX](MCP4XXX/)**: I2C Digital Potentiometer library for Microchip MCP4XXX series.

Each library folder contains:
- Source files (`.h`, `.c`)
- Platform abstraction files
- Device-specific README.md with usage and integration instructions

## Getting Started
1. Clone the Repository:

```bash
git clone https://github.com/Darvock2640/FirmwareForge.git
```

2. Navigate to the Library:

```bash
cd FirmwareForge/ADS8866
# or
cd FirmwareForge/MCP48FVXX
# or
cd FirmwareForge/adc124s021
```

3. Integrate into Your Project:

- Copy the library's ```.h``` and ```.c``` files into your project's source directory.

- Include the header file in your main source file: ```#include "library_name.h```

- Compile and link the source files with your project.

## Usage
Each library has its own README.md in its directory with specific usage instructions and examples.

## Contributing
We welcome contributions from the community! If you'd like to contribute, please follow these steps:

1. Fork the repository.

2. Create a new branch (git checkout -b feature/your-feature-name).

3. Implement your changes and write tests if applicable.

4. Commit your changes (git commit -m 'feat: Add new feature').

5. Push to the branch (git push origin feature/your-feature-name).

6. Open a Pull Request.

Please ensure your code follows the existing style and is well-documented.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
