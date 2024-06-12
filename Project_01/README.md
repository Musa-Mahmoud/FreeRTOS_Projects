# STM32F446RE CLI Program

## Overview

This project is a Command Line Interface (CLI) program designed for the STM32F446RE microcontroller. It leverages FreeRTOS objects such as queues, direct to task notifications, and software timers combined with the STM HAL library to control various peripherals of the microcontroller. The program provides a user-friendly menu system to manage LEDs, UART communication, and the Real-Time Clock (RTC).

## Features

- **FreeRTOS Integration**:
    - Utilizes queues, direct to task notifications, and software timers for efficient task management and scheduling.
- **Peripheral Control**:
    - **GPIO**: Control LEDs by turning them on/off and creating various LED effects.
    - **UART**: Send characters to the console for communication and control.
    - **RTC**: Configure and retrieve the current time and date.
- **Menu System**:
    - **Main Menu**: Access sub-menus for LED effects, date and time settings, or exit the program.
    - **LED's Effect Menu**: Choose from different LED effects controlled by software timers.
    - **Date and Time Menu**: Configure the RTC settings and report the current date and time periodically.

## Menu System

### Main Menu

- **1. LED's Effect Menu**: Navigate to the LED's effect menu.
- **2. Date and Time Menu**: Navigate to the date and time menu.
- **3. Exit Program**: Exit the CLI program.

### LED's Effect Menu

- **1. Stop Effect**: Stop any running LED effects.
- **2. Toggle All LEDs Effect**: Toggle all LEDs on and off.
- **3. Even-Odd Effect**: Toggle LEDs in an even-odd pattern.
- **4. Shift Right Effect**: Shift the LED pattern to the right.
- **5. Shift Left Effect**: Shift the LED pattern to the left.

### Date and Time Menu

- **1. Configure Time**: Set the current time.
- **2. Configure Date**: Set the current date.
- **3. Report Date and Time**: Report the current date and time to ITM every second using software timers.
- **4. Back to Main Menu**: Return to the main menu.

## Getting Started

### Prerequisites

- **Development Board**: STM32F446RE
- **Toolchain**:
    - ARM GCC Compiler
    - STM32CubeMX for code generation
    - STM32CubeIDE or any preferred IDE for development
- **Libraries**:
    - FreeRTOS
    - STM32 HAL Library.

### Usage

1. **Connect to the Board**:
    - Use a serial terminal application (e.g., PuTTY, Tera Term) to connect to the UART interface of the STM32F446RE.
2. **Navigate the CLI**:
    - Use the main menu to navigate to sub-menus for LED effects and date/time settings.
    - Execute commands by selecting the corresponding menu option.
