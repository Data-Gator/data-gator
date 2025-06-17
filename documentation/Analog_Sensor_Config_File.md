This document explains how to create and use the `analog_config.json` file for assigning analog sensors to specific ports on the Data-Gator. This configuration file is read from the SD card at runtime and is used by the firmware to initialize the appropriate sensor drivers.

---

## File Name and Location

- **File name:** `analog_config.json`
- **Location:** Root directory of the SD card (e.g., `/analog_config.json`)

> **NOTE:** When copying the file to your SD card, place the file directly on the card without any folders.

---

## File Format

The file must be in standard JSON format. The analog sensor configuration should be stored under the key `analog_ports`, where each key-value pair maps a port number (as a string) to a sensor type (as a string).

### Example File:

```json
{ 
    "analog_ports": {
        "port_shallow": "MIJ02LMS",
        "port_middle": "NONE",
        "port_deep": "NONE", 
        "port_analog": "NONE" 
    }
}
```

---

## Currently Supported Sensor Types

You can assign any of the following sensor types to a port:

| Type       | Description                       |
| ---------- | --------------------------------- |
| `NONE`     | No sensor connected to this port  |
| `TEROS_10` | Teros 10 VWC Soil Moisture Sensor |
| `MIJ02LMS` | MIJ-02-LS Dendrometer             |
 

---

## Default Behavior if File is Missing

If the `analog_config.json` file is not found on the SD card, the firmware will fall back to a default configuration as found in previous firmware versions. In this case:

- `port_shallow` will default to `TEROS_10`
- `port_middle` will default to `TEROS_10`
- `port_deep` will default to `TEROS_10`
- `port_analog` will default to `NONE`

This ensures that basic VWC sensing functionality is still available even if the configuration file is missing or unreadable, and it preserves backwards compatibility with previous firmware editions.

---

## Troubleshooting

- Make sure the SD card is inserted and the file is in the root directory.
- Ensure the file is valid JSON. You can use [https://jsonlint.com](https://jsonlint.com) to validate.
- Double-check the spelling of each sensor type. Strings like `MIJ02LMS` are not valid unless explicitly added in the firmware.

---

## Summary

1. Create a file called `analog_config.json`.
2. Add an `analog_ports` object with port-to-sensor mappings.
3. Copy the file to the root of your SD card.
4. Insert the SD card into the ESP32 device and reboot.

That's it! The firmware will now configure the analog sensors based on your JSON file.