# Basic Tests
Verify that all major modules initialize and return realistic values.

#### Tested Systems
* SPI(microSD)
* I2C(ADC & Fuel Gauge)
* Analog Pins(mode select DIP Switch)
* WiFi 

#### Usage

| Command | Description | 
| :-----: | :---------: |
| `pio test -f test_basic` | run only the basic tests to verify that the hardware is working correctly | 
