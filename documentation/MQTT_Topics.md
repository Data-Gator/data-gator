# MQTT Topics
**How to use this documentation:** a brief summary of each topic is listed below in the [MQTT](#mqtt) section. Each entry/topic in that section will also have expanded documentation below with message fields, structure, and default value information in [Message Documentation](#message-documentation).

If you are looking for documentation pertaining to the MQTT broker configuration used and recommended for this project, see  [this page](https://github.com/Project-VineHeart/mosquitto_config)


## MQTT
Data is logged to several topics listed below:

#### Sensor Topics
|Name | Topic | Description |
| :---: | :---: | --- |
| VWC | `<brand_sensormodel>/<sensor_depth>/<DG_mac_addr>` | message contains volumetric water content for `shallow\|middle\|deep` sensor, readings are relative with 100% representing pure water
| | | `{"MAC": "<mac_addr>", "VWC":<float>, "VWC_RAW":<float_voltage>, "DEPTH":"<shallow\|middle\|deep>"}`
| HT(temp and humidity) | `<brand_snesormodel/<sensor_mac_addr>` | relative humidity and temperature readings from a wireless sensor
| | | `{"MAC": "<sensor_mac_addr>", "GATOR_MAC":<DG_mac_addr>, "HUMIDITY":<float>, "TEMP":<float_in_C>, "SENSOR_NAME": "<sensor_name_str>", "BATT_VOLTAGE": <int_in_mV>}`
| PH | `brand_sensormodel/pH/<DG_mac_addr>` | pH reading, taken by a Data Gator
| | | `{"MAC":"<dg_mac_addr>", "PH":<float>, "PH_RAW":<float_voltage>}`


#### Data Gator Data Topics
|Name | Topic | Description |
| :---: | :---: | --- |
| TLM | `datagator/tlm/<DG_mac_addr>` | telemetry information for a given Data Gator device containing information such as battery charge and connection strength
| | | `{"MAC":"<dg_mac_addr>", "BATT_VOLTAGE":<float>, "FIRMWARE_VERSION":"<major>.<minor>.<patch>v"}`

#### Data Gator Commands
|Name | Topic | Description |
| :---: | :---: | --- |
| Data Request Command | `datagator/cmd/get_time_range/<DG_mac_addr>` | request data logged to the SD card during a specified time range be reported via MQTT to the broker
| | | `{"PAGE_SIZE": 50, "TIME_RANGE":"<month>-<day>-<year>T<hr>:<min>:<sec>&<month>-<day>-<year>T<hr>:<min>:<sec>", "TOPIC_FILTER":[""]}`


#### Data Gator Command Responses
|Name | Topic | Description |
| :---: | :---: | --- |
| Data Request Response | `datagator/data/time_range/<DG_mac_addr>` | data published by the data gator, should be broken into multiple messages, published as pages of length specified by the user
| | | `{"file_name":"<filename>", "epoch":<long int>, "terminus":<long int>, "data":["<str>"]}`


## Message Documentation 
| Name | Message Structure | 
| --- | --- | 
| VWC | `{"MAC": "<mac_addr>", "VWC":<float>, "VWC_RAW":<float_voltage>, "DEPTH":"<shallow\|middle\|deep>"}`
| HT(temp and humidity) | `{"MAC": "<sensor_mac_addr>", "GATOR_MAC":<DG_mac_addr>, "HUMIDITY":<float>, "TEMP":<float_in_C>}`
| Gator TLM | `{"MAC":"<dg_mac_addr>", "BATT_VOLTAGE":<float>, "FIRMWARE_VERSION":"<major>.<minor>.<patch>v"}`
| PH | `{"MAC":"<dg_mac_addr>", "PH":<float>, "PH_RAW":<float_voltage>}`
| Data Request Command | `{"PAGE_SIZE": 50, "TIME_RANGE":"<month>-<day>-<year>T<hr>:<min>:<sec>&<month>-<day>-<year>T<hr>:<min>:<sec>", "TOPIC_FILTER":[""]}`
| Data Request Response | `{"file_name":"<filename>", "epoch":<long int>, "terminus":<long int>, "data":["<str>"]}`
