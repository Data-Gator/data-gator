# Data Logging System

## Overview
"Data Logging System" refers to all mechanisms and code used to record data collected by the Data Gators (DGs). Such data is currently stored in one of two ways:

1. On the server via WiFi connection to MQTT broker
2. On the SD card which in text files with entries keyed by timestamp and data consisting of JSON formatted strings.


## Data Retrieval and Querying
Currently the MQTT data uploaded to the server can be queried, but the DGs do not support the querying of data stored on the SD cards, which is the case when there is no WiFi connection.

#### Fields
Below are fields which can be used to define which data to return. These fields are keys in the JSON object which will be sent to the DGs as a query body. If a field is not present, the default value below will be assumed.

| Field | Description/Format | Default Value |
| :---: | :---: | :---: | 
| Date Range | One to two dates of the format: "<month>-<day>-<year>T<hr>:<min>" in a list. | No default value, this must be present. |
| Topic List | List of topics (white list) to return. | "" a.k.a empty string to signify return all topics. |

#### Functionality Required for Querying Data
The functionality and planned modules are listed and described below.

1. A standard interface between mqtt broker and client
    * interface should define fields for requesting data from the device
    * needs to have QoS level which will facilitate operation regardless of whether DG is in sleep state or awake and available to service requests.

2. A library which will take a set of retrieval parameters specified by the interface in step one, and return matching data (or error message for possible failures).

3. A data structure which will contain retrieved data as well as metadata about query parameters.

4. A return JSON structure transmitted via MQTT.
    * is it one chunk?
    * is the data split("paginated") to reduce memory load?
    * what happens if queried data is too large for memory constraints?


