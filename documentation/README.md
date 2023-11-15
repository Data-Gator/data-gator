# Data Gator Documentation
This directory contains reference documentation for the data gator firmware solution. See the sections below for:

1. a high-level hardware description,
2. a firmware architecture description,
3. flowcharts!

##### Table of Contents

- [Firmware API Documentation](https://data-gator.github.io/doxygen_firmware_docs/index.html)
- [Building and Flashing Firmware](How_to_Flash_Firmware.md)
- [Configuration Files and Profiles](Configuration_Files_and_Creating_Profiles.md)
- [How to Make Firmware Releases](./how_to_make_releases.md)
- [Espressif Flash Tool](Espressif_Flash_Download_Tool.md)
- [How to Access Logged Data](How_To_Access_Logged_Data.md)
- [MQTT Topics](MQTT_Topics.md)
- [Sensor Guides](sensors_and_wiring/README.md)

# Workflow for Users

_**Pre-requisites:** A data gator board with sensors and power supply. If you don't have these or want to know more about setting them up, see the hardware documentation in [github pages](https://data-gator.github.io/Hardware)_

1. Connect computer to board via USB. 
2. Install platformio with command line interface (CLI). Open a terminal and clone the firmware repository, then initialize all git submodules with the commands below:

        // clone firmware and these docs
        git clone https://github.com/Data-Gator/data-gator.git
        // add submodules and update them
        git submodule update --init --recursive

3. Use platformio for any of the following tasks:

    * [building/compiling code](How_to_Flash_Firmware.md) 
    * [building/compiling & uploading to Data Gator/uC](How_to_Flash_Firmware.md)
    * [select configuration options for board](Configuration_Files_and_Creating_Profiles.md)
    * [use unit tests to check functionality]()
    * [use espressif flash tool to flash pre-built firmware](Espressif_Flash_Download_Tool.md)

# Workflow for Firmware Developers

_**Pre-requisites:** A data gator board with sensors and power supply. Also some knowledge of C++ & Arduino concepts. If you don't have these or want to know more about setting them up, see the hardware documentation in [github pages](https://data-gator.github.io/Hardware)_

It is assumed if you are a developer, you are interested in doing one of the following or have the skills to do whatever you want.

* [Doxygen Documentation for API](https://data-gator.github.io/doxygen_firmware_docs/index.html)

    * if you have doxygen installed on your system, build doxygen docs from code with `doxygen datagator.doxyconfig` from root of repository.

* [Guide for Firmware Releases and the Version Tracking System](how_to_make_releases.md)
* [Guide for Using the Configuration Scripts](Configuration_Files_and_Creating_Profiles.md)
* [MQTT Topics Documentation](MQTT_Topics.md)

# Architecture Description

### High-Level Hardware Description
Hardware documentation in [github pages](https://data-gator.github.io/Hardware).

1. The Micro-Controller(MCU)
    * currently an ESP32 based MCU with WiFi, BLE, and GPIO for interfacing with sensors
    * soldered to a carrier board which provides expanded functionality and better connection options 

2. The Carrier Board(Data Gator)
    * soldered to the MCU, it provides an improved Analog-to-Digital-Converter(ADC), Multiplexer(MUX), power management chips, and Watch-Dog Timer(WDT)
    * the carrier board also provides screw-terminal interfaces for robust and re-usable connection to wired sensors 

3. The Solar System
    * intended to be used with a battery, solar power manager, and solar panel to provide year-round sustainable operation
    * also includes a fuel gauge solution(MAX17048) to monitor battery charge level
    * requires a 1-2 Watt solar panel

### Firmware Configuration and Usage
The firmware is based around a modular architecture which is divided into several main parts:

1. Sensor Interfaces
    * sensor interfaces are defined for Bluetooth Low Energy temperature and humidity sensors, Volumetric Water Content(VWC) sensors, and pH sensors
    * sensor interfaces are based around the protocol/connection type that they use(I<sup>2</sup>C, BLE, analog, etc)
    * sensor interfaces can be easily modified to add support for new sensor hardware without substantial changes to other firmware code - just make sure the MQTT subscriber picks up the new topics! 

2. Task Scheduling
    * one task is defined for each sensor that the aggregator is connected to 
    * `config.hpp` defines the polling frequency for the tasks in minutes, but should not be edited directly if possible. It is better to follow the instructions from the guide in the table of contents above titled "Configuration Files and Profiles"
    * the MCU is woken from deep sleep by the WDT to check the scheduling table and execute scheduled tasks

3. Data Logging
    * the logging module supports automatic logging to any of the specified interfaces such as Serial connection, MQTT(WiFi), SD card, and potentially LoRa
    * logging destination is automatically chosen by the module based on what interfaces are available

Further documentation and usage information will be defined in [firmware_documentation.md](firmware_documentation.md). A flowchart of the modules can be seen below.

![png](images/firmware_chart.png)

### Hardware Debugging and Documentation
Hardware characteristics of the system will be documented in [MQTT_Hardware_Documentation.xlsx](MQTT_Hardware_Documentation.xlsx), especially during development as new features are being added.

Stable feature documentation will later be accessible through [hardware_documentation.md](hardware_documentation.md).

![png](images/hardware_flow_chart.png)

