# How to Flash Firmware
This is a guide for how to flash firmware images onto ESP32 mounted on the Datagator board. It contains all information from guidelines released with each firmware release as well as additional commands and techniques for troubleshooting.

In this guide, flashing refers to uploading an executable(binary) file to the ESP32 micro-controller. It is possible to do this with two different tools:

1. [PlatformIO](#platformio) This method is most tightly integrated with the firmware development environment used by the developers and is easiest when it is necessary to build from source and then upload. It can be used through both command line and visual studio.
2. [Espressif Flash Tool(Windows)](#espressif-flash-download-tool) Using the espressif flash tool is the best and only method for flashing pre-compiled executable files included with releases. Unfortunately this is only available for Windows.


# PlatformIO

##### PlatformIO Options
With laptop connected to Data Gator, build and upload the source code with:

1. VSCode Plugin
2. OR use platformio CLI tool `pio`

Currently, it doesn't appear possible to flash pre-built firmware images to a board without building in PlatformIO. For mass production, it is recommended that you use the Espressif Flash Tool.

## PlatformIO with VSCode

## PlatformIO CLI

1. Simple compile and upload (flash): `pio run -t upload`

2. For serial monitor use: `pio run -t upload -t monitor`

3. To build with different configuration options: `pio run -t debug`

    * change how often sensors read, network options, etc


###### Note
There are many other options for PlatformIO's CLI and some configuration scripts are integrated to help manage projects. Instructions for using the configuration manager system and version control system for Data Gator firmware releases can be found under [developer workflows](./README.md#workflow-for-firmware-developers).


# Espressif Flash Download Tool
To flash the linked firmware, use the Espressif Flash Download Tool(Windows) from [here](https://www.espressif.com/en/support/download/other-tools).

1. Download and open Flash Download Tool. Also download the binary file (`.bin`) linked below.
2. Select ESP32 as the chip. Leave all other options as default and click "Okay".
4. Open the binary file by clicking the three dots button next to the top entry. For each binary loaded, enter an offset (ex. `0x10000`) in the far right field on the same line. This sets the program to be written to the memory block.

    - `bootloader.bin` should be flashed with offset `0x1000`
    - `partitions.bin` should be flashed with offset `0x8000`
    - `firmware_v#.#.#.bin` should be flashed with offset `0x10000`

6. Click the check box next to the file selector field for each loaded binary to be flashed.
7. Next, connect the Data Gator board via USB to your laptop. Open device manager to see which COM port the USB device is connected to.
8. Select that COM port in the Flash Download Tool.
9. Click "Start". The tool should SYNC, then flash the firmware to the device. Do not disconnect until it is done. Once finished, use the PlatformIO serial monitor or other serial monitor to make sure the device is running the correct firmware.
