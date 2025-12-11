# DS50 Demo Application

## Overview

This is a demo application for the DS50 payment terminal. The application provides a complete payment solution with support for:

- Card payments (EMV chip and magnetic stripe)
- QR code transactions (both static and dynamic)
- MQTT connectivity for remote management
- Audio prompts and visual feedback via LCD
- Network connectivity (WiFi/Ethernet)

## Features

### Payment Processing

- EMV Level 2 compliant card payment processing
- Support for contact (chip) and contactless (RF) cards
- Magnetic stripe card reading
- Secure transaction handling with encryption

### QR Code Transactions

- Static QR code generation for fixed merchant scenarios
- Dynamic QR code generation for variable amount transactions
- Integration with Dspread payment gateway

### Connectivity

- WiFi and cellular network support
- MQTT protocol for remote management and updates
- HTTP/HTTPS communication for transaction processing

### User Interface

- Graphical LCD interface using LVGL (LittlevGL)
- Audio feedback through built-in speaker
- Keypad input handling
- Visual indicators for transaction status

## Hardware Platform

- **File system**: Embedded file system
- **Connectivity**: WiFi, optional cellular module
- **Display**: Lvgl interface
- **Audio**: Built-in speaker and microphone
- **Card Reader**: EMV chip reader, magnetic stripe reader

## Directory Structure

```
.
├── app/                    # Application source code
│   ├── inc/                # Header files
│   └── src/                # Source files
│       ├── app/            # Application modules
│       ├── emqx/           # MQTT implementation
│       ├── pub/            # Public utilities
│       └── main.c          # Entry point
├── cmake/                  # CMake build configuration
├── components/             # System components and libraries
├── dspread/                # Dspread SDK and libraries
├── ldscripts/              # Linker scripts
├── prebuilts/              # Prebuilt tools and binaries
└── tools/                  # Build tools and utilities
```

## Key Components

### Core Modules

- [main.c](app/src/main.c) - Application entry point and main thread
- [app_lvgl.c](app/src/pub/app_lvgl.c) - LVGL graphics interface implementation
- [app_trans.c](app/src/app/app_trans.c) - Transaction processing logic
- [app_card_payment.c](app/src/pub/app_card_payment.c) - EMV card payment handling
- [app_emqx.c](app/src/emqx/app_emqx.c) - MQTT connectivity implementation

### Libraries

- **Dspread SDK**: Payment processing libraries
- **LVGL v7**: Graphics library for UI rendering
- **mbedTLS**: Cryptographic functions
- **EMV Kernel**: EMV Level 2 payment processing

## Build Instructions

### Prerequisites

- Windows OS (tested on Windows 10/11)
- ARM GCC toolchain
- Python 3.x
- CMake 3.13 or higher
- Ninja build system

### Building

1. Double-click [Build.bat](Build.bat) to compile the application
2. The build output will be generated in `out/appimage_debug/hex/`
3. The main output files are:
   - `DS50_app.img` - Application image
   - `DS50_app.pac` - Packaged firmware file

### Build Process

The build script automatically:

1. Sets up the required environment paths
2. Configures the CMake build system
3. Compiles the source code using Ninja
4. Links all required libraries
5. Generates the final application image

## Flashing Firmware

After successful compilation:

1. Use the generated `.pac` file with the appropriate flashing tool
2. Connect the DS50 device via USB
3. Flash the firmware using the manufacturer's flashing utility

## Configuration

### Network Settings

Network configuration can be adjusted in the source code:

- WiFi SSID and password
- Server URLs for payment processing
- MQTT broker settings

# 

## Development Guidelines

### Adding New Features

1. Create new modules in the appropriate directory under [app/src/](app/src/)
2. Add header files to [app/inc/](app/inc/)
3. Update [CMakeLists.txt](CMakeLists.txt) to include new source files
4. Follow existing code patterns and naming conventions

### Code Structure

- Use modular design with clear separation of concerns
- Implement proper error handling
- Follow C coding standards
- Use the existing logging mechanism (`API_LOG_DEBUG`)

## Troubleshooting

### Common Build Issues

1. **Missing tools**: Ensure all prebuilt tools are in place
2. **Path issues**: Check that no spaces exist in the project path
3. **Permission errors**: Run the build script as administrator if needed

### Runtime Issues

1. **Network connectivity**: Verify WiFi credentials and signal strength
2. **Card reading failures**: Clean card reader and check card orientation
3. **Display issues**: Confirm LCD initialization and cable connections

## License

This project is provided as-is without warranties. Please refer to individual component licenses for details.

## Support

For technical support, please contact the device manufacturer or refer to the official documentation.