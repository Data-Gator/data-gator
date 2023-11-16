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
- [Firmware States and Execution Description](firmware_documentation.md)

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

## Quick Links
It is assumed if you are a developer, you are interested in doing one of the following or have the skills to do whatever you want.

* [Detailed Firmware Execution Description](firmware_documentation.md)
* [Extending Firmware Functionality: New Sensors and Other Fun!](extending_firmware_functionality.md)
* [Doxygen Documentation for API](https://data-gator.github.io/doxygen_firmware_docs/index.html)

    * pre-built doxygen docs are available at the link above, but if you have doxygen installed on your system, build local doxygen docs from code with `doxygen datagator.doxyconfig` from root of repository.

* [Guide for Firmware Releases and the Version Tracking System](how_to_make_releases.md)
* [Guide for Using the Configuration Scripts](Configuration_Files_and_Creating_Profiles.md)
* [MQTT Topics Documentation](MQTT_Topics.md)


### Changing Configuration Options and Building a New Firmware Image

1. Edit `config.ini`. This file can be used to **add** global parameters or **modify** the values of default global definitions. At compile time, options from `config.ini` are used to generate `include/config.hpp` so that the parameters are available in the binary. _(default parameters listed below)_

    * Network ID
    * Network Password
    * MQTT Broker IP Address & Port 
    * Serial Debug Flag 
    * WiFi Connection Timeout
    * OTA Update Frequency
    * Other Task Frequencies

2. Run config generator script with:

        // to generate config.hpp
        pio run -e debug

3. Use script interface to select a configuration option from menu options which are loaded from `config.ini`. If no options are available, you want to add a configuration, or you want to change some values: repeat step one or refer to [the config guide](./Configuration_Files_and_Creating_Profiles.md).


###### Note
Until the config generator script is run again, every time the firmware is compiled, the values in `include/config.hpp` will be used. Open `config.ini` to check available values or open `include/config.hpp` to check what values are being compiled for use.

* if the script runs, you should see something similar to:

    ![config gen ascii](images/config_generator_output.png)
        

### Creating a New Firmware Release/Version

If creating a new a new firmware version/release is desired it is necessary to:

1. Choose configuration settings using the [configuration workflow](#changing-configuration-options-and-building-a-new-firmware-image).

2. Increment the firmware version number and build firmware image binary. For example if the current version is v1.0.12 the next version could be v2.0.0 (major release), v1.1.0 (minor release), or v1.0.13 (patch release). To automatically increment version number, run one of the following:

        // for a patch release (bug fixes)
        pio run -e rpatch

        // minor (non-api breaking changes)
        pio run -e rminor 

        // major (api breaking changes and major functionality upgrades)
        pio run -e rmajor

        // last resort to revert undesirable version and choose major, minor, and patch version manually
        // good for fixing oopsies!
        // warning overwrites existing version info!
        pio run -e rmanual

    - in this step, the old version number is retrieved from `include/version.hpp`, incremented and written back
    - if manual is used, the old version number is simply overwritten

3. Get generated binary from `.pio` directory.

    - after step 2, the binary file will be placed in `.pio/build/<rmajor|rminor|rpatch>/datagator_v<major>.<minor>.<patch>.bin`

### Adding and Integrating Support For a New Sensor

Writing firmware to integrate a new sensor requires the following steps:

1. Choose a sensor interface to extend or write your own and place it in `include/<sensor_interface>.hpp`. For example, the following are pre-defined interfaces that can be used:

    - [VWCSensor](https://data-gator.github.io/doxygen_firmware_docs/classVWCSensor.html)
    - [BLESensor](https://data-gator.github.io/doxygen_firmware_docs/classBLESensor.html)
    - [pHSensor](https://data-gator.github.io/doxygen_firmware_docs/classpHSensor.html)

2. Next, write a sensor library defining a custom class which inherits from the chosen interface. Your library should be placed in `lib/<sensor_library>`.

        lib/
            |
            |
            +--- <sensor_name>/
                    |
                    +--- README.md          # documentation here, including links to product page, manufacturer guides and documentation
                    +--- <sensor_name>.hpp  # doxygen style comments preferred for incorporation into auto generated documentation, but not required
                    +--- <sensor_name>.cpp

3. Integrate sensor library with Data Gator task scheduling mechanism.
    
    - Currently the firmware defines four tasks which are run periodically based on [`include/config.hpp`](https://data-gator.github.io/doxygen_firmware_docs/config_8hpp.html) parameters.

    - Code for reading from a new sensor needs to be integrated (placed) in the one of these tasks, or a new task should be defined for the scheduler to handle and integrated into the [planner struct](https://data-gator.github.io/doxygen_firmware_docs/structplanner.html).

        - _**NOTE:** The caveat to this rule is that BLE sensors should be integrated into [void ScanCallbacks::onResult(...)](https://data-gator.github.io/doxygen_firmware_docs/classScanCallbacks.html) since BLE sensors are detected by the BLE stack and their data packets parsed within this function._

    - Tasks are defined in [`include/scheduler.hpp`](https://data-gator.github.io/doxygen_firmware_docs/scheduler_8hpp.html).

4. Define the MQTT topic and message structure for logging/reporting readings from your sensor. You can see examples [here](MQTT_Topics.md).

    - Notice that pre-defined sensor types and interfaces also define a set topic and message structure. In the next step you will write the functions used to fill in all fields such as `<brand_sensormodel>` or `<DG_mac_addr>`.
        
        - It is acceptable to **add** more data fields to the message, but to prevent missing key errors pre-defined fields **must** be present in pre-defined topics.

        - When defining a new topic and message structure here are some guidelines:

            1. Only define a new type of sensor topic if the type of data collected by the sensor is different.
            2. Include identifying data in the topic. For example, if the sensor doesn't provide unique identification data such as a MAC address, use the Data Gator's MAC address as the last topic path field to identify the sensor (ex.  datagator/tlm/<DG_mac_addr>).
            3. Include identifying information, raw data (voltage or unscaled version), and scaled/converted data in the message body. Including identifying information in the body helps containerize information. Including the unscaled as well as scaled data allows for data cleaning, calculation verification, and other perks.
            
5. Define member methods for constructing a MQTT style topic and message. Please follow the convention of overwriting the following methods to provide this functionality:

    - `std::string getSensorType()`

        - _Example:_ [std::string BLESensor::getSensorType()](https://data-gator.github.io/doxygen_firmware_docs/classBLESensor.html)

        - _`getSensorType` returns the (hopefully unique) name of the sensor so that its data can be identified and tracked_
        
    - `std::string toJSON(_data here_)`

        - _Example:_ [std::string Teros10::toJSON(double voltage)](https://data-gator.github.io/doxygen_firmware_docs/classTeros10.html#a3396ce8a397fcb209bcfd041dc561764)

        - _`toJSON(...)` converts from a raw data reading (taken from a hardware resource such as a voltage from the ADC) and converts that value to a string of JSON fields ready to be injected into a JSON object for data logging.

            - This function is likely calling an underlying conversion function such as [`double Teros10::getVWC(...)`](https://data-gator.github.io/doxygen_firmware_docs/classTeros10.html#ad5870a7c628c5531b87d687dee857836) to convert the voltage to a soil water content reading. 

            - Once the voltage has been scaled/converted it is converted to a string so that it can be placed in a JSON object.


6. Log the JSON data using the logging utility: [`log_data(string topic, string message)`](https://data-gator.github.io/doxygen_firmware_docs/logger_8hpp.html#a2799093145858620308ff4421e6a1ea3).
7. Test! Your message should be logged to the serial monitor if the `USB_SERIAL` flag is raised and will be logged to MQTT if there is a WiFi connection and broker.


