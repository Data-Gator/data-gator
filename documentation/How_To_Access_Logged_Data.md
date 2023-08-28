# How to Access Logged Data 
The Data Gator logs data from sensors in three ways:

1. _**MQTT:**_ data is published to a broker where it can be collected and stored in a database for processing and archival.
2. _**SD Card:**_ if no path to the MQTT broker configured for the Data Gator is available, it will default to logging data to a memory card if connected in CSV files
3. _**Serial:**_ it is possible to record data via the serial connection using PlatformIO's serial logging capability, though the data is not printed in a format which is easy to process

## MQTT
Data is logged to several topics listed below:

#### Sensor Topics
|Name | Topic | Description |
| :---: | :---: | --- |
| VWC | `<brand_sensormodel>/<sensor_depth>/<DG_mac_addr>` | message contains volumetric water content for `shallow\|middle\|deep` sensor, readings are relative with 100% representing pure water
| HT(temp and humidity) | `<brand_snesormodel/<sensor_mac_addr>` | relative humidity and temperature readings from a wireless sensor
| PH | `brand_sensormodel/pH/<DG_mac_addr>` | pH reading, taken by a Data Gator


#### Data Gator Data Topics
|Name | Topic | Description |
| :---: | :---: | --- |
| TLM | `datagator/tlm/<DG_mac_addr>` | telemetry information for a given Data Gator device containing information such as battery charge and connection strength

#### Data Gator Commands
|Name | Topic | Description |
| :---: | :---: | --- |
| Data Request Command | `datagator/cmd/get_time_range/<DG_mac_addr>` | request data logged to the SD card during a specified time range be reported via MQTT to the broker


#### Data Gator Command Responses
|Name | Topic | Description |
| :---: | :---: | --- |
| Data Request Response | `datagator/data/time_range/<DG_mac_addr>` | data published by the data gator, should be broken into multiple messages, published as pages of length specified by the user


## SD Card/CSV Data
Data logged to the SD card is stored in files according to the time the data was collected. Files are marked `data_<date_collected>.csv`. Three fields are stored in the file, separated by `;`:

1. _**Time stamp:**_ a time stamp with the format `mm-dd-yyThh:mm:ss+<offset>` where everything left of `+` is a conventional time stamp, and the `<offset>` is the number of minutes since the last network connection was available to acquire a true time stamp.
2. _**Topic:**_ the MQTT topic this record was supposed to be published to
3. _**Message:**_ JSON formatted string with message body/payload that should have been published


#### Retrieving Data
Two methods are available for retrieving the data. The first is the manual access method. Remove the SD card from the device and use a computer to copy/open the CSV files. The second is to publish a data request command, listed above in the MQTT documentation, and wait for the response to be published to the corresponding MQTT broker response topic.


## Serial Data Logging
PlatformIO facilitates saving data from the serial monitor to a time stamped log file. To use this feature follow the instructions for using `log2file` filter on [this page](https://docs.platformio.org/en/latest/core/userguide/device/cmd_monitor.html)

