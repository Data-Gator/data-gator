# Espressif Flash Download Tool
To flash the linked firmware, use the Espressif Flash Download Tool(Windows) from [here](https://www.espressif.com/en/support/download/other-tools).

1. Download and open Flash Download Tool. Also download the binary file (.bin) linked below.
2. Select ESP32 as the chip. Leave all other options as default and click "Okay".
3. Open the binary file by clicking the three dots button next to the top entry.
4. Next, connect the Data Gator board via USB to your laptop. Open device manager to see which COM port the USB device is connected to.
5. Select that COM port in the Flash Download Tool.
6. Click "Start". The tool should SYNC, then flash the firmware to the device. Do not disconnect until it is done. Once finished, use the PlatformIO serial monitor or other serial monitor to make sure the device is running the correct firmware.

