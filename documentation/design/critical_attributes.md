# Critical System Attributes
The critical attributes of the aggregators for Project Vineheart are defined here. Under each section there will be a short description of the functionality needed(problem statement), with parameters describing the critical attributes of acceptable solutions.

## Attribute List/Critical Functions
These are the critical functions which the data aggregator solution must provide without any parameters for functionality.

1. Soil Moisture/Volumetric Water Content measuring solution.
2. Temperature and Humidity measuring ability.
3. Wireless solution support for sensors and the ability to report sensor readings via MQTT to a broker over WiFi.
4. Solar, rechargeable battery power system which enables reliable operation year round. 

The following sections define characteristics by which the solution for each attribute may be judged. If the minimum specified performance characteristics below are not met the solution may be considered a failure.


# Wired Interfaces
**What:** Supports the ability to connect pH and VWC sensors for measuring soil state. 

**Minimum Support:** Three analog sensors for taking readings across three soil levels in a single location. Will also expose multi-device interfaces(SPI, I2C, WiFi, BLE) for alternative options.

**Analog Accuracy Requirement:** Aggregator board will provide voltage readings accurate to +/- 0.015V(taken from tolerance of atlas pH meter) or minimum of 12bit ADC resolution(taken from Meter Teros 10 specification).

**Fault Detection:** Device will be capable of reporting if sensor is unconnected to port. Analog interfaces will pull lines low unless connected to sensor.

**Power Consumption:** Wired sensors will be capable of being fully turned off when system is conserving power and will draw no more than 1A or the maximum supplied amperage of the system's GPIO.

# Wireless Interfaces
**What:** System supports both WiFi and BLE interfaces for communicating with sensors and report data to the user.

**Minimum BLE Support:** System will be capable of scanning sensors within _**X**_ ft within _**Y**_ seconds provided that the sensors are broadcasting with a frequency > once every _**Y**_ seconds. _base this on sensor density??_

**Minimum WiFi Support:** System will be capable uploading data to and interfacing with gateways within _**X**_ ft given a line of sight connection. 

**BLE Reliability:** System will read from _**X**_ sensors at least once when waking from sleep to poll BLE sensors. Reliability will decrease above this mark.

# Solar Power System
**What:** System is reliably powered year-round by a solar power system capable of reporting its charge state to the user. Normal System operation will expect to read from all sensors every 5 minutes.

**Minimum Reliability:** Able to maintain nominal sensor readings from all sensors with a frequency of once every 5 minutes for at least 365 days with an average of 156 days of sunshine every year(3 days of sun per week).

**Minimum Battery Life:** Battery capable of sustaining normal operation for 5 days without sunshine. _may need to extend this_

**Minimum Recharge Ability:** System should be capable of recharging fully with 2 days of sunshine with [3.5 peak hours of sunshine](https://unboundsolar.com/solar-information/sun-hours-us-map).
