# Configuration Files and Creating Configuration Profiles
This guide addresses the configuration system employed for creating configuration profiles for systems with different network authentication settings, sensor timing requirements, MQTT broker configurations, etc.

This configuration guide should not be confused with the PlatformIO configuration (`platformio.ini`) because those files contain build system parameters and automated script configurations. This configuration guide addresses how to configure data gator functionality for different devices or sensor networks.

## How It Works

1. `config.ini` contains configuration profiles created by users. 
2. `pio -e debug -t upload` runs a configuration script which allows the user to select a configuration profile from `config.ini`.
3. Parameters from the selected profile are used to generate a configuration header which is written to `include/config.hpp`.
4. Finally, you may need to `CTRL-C` to exit the automated script, but the project will compile from source with your configuration profile of choice and be uploaded to the board (`-t upload` option).


## Default Configuration Profile 
The options below will appear in `include/config.hpp` with the values below if not overridden by a chosen profile.


```
[DEFAULT]
s_network = RPiHotspot
s_psswd = 1234567890

s_mqtt_broker_addr = 192.168.50.10
i_mqtt_port = 1883

; serial debug messages are turned on
i_debug = 1

; wait ten seconds for network connection
i_wifi_timeout = 10000
i_max_count = 120

; sensor reading frequency
i_ota_freq = 60
i_vwc_freq= 1
i_ht_freq = 1
i_tlm_freq = 1
```

## Configuration File Syntax
Although based on python's [`configparser`](https://docs.python.org/3/library/configparser.html) library, a typing convention is implemented to help the configuration generation script interpret the type of each option and convert to valid C++ syntax.

```
s_network = RPiHotspot
```

becomes

```c++
#define NETWORK "RPiHotspot"
```

and 

```
i_mqtt_port = 1883
```

becomes

```c++
#define MQTT_PORT 1883
```

As you can see, the C++ version of each option is typed according to the leading character of the option name. `i` denotes an integer while `s` denotes a string wrapped in quotation marks. Following this convention, you can add your own options as desired and they will appear in the generated `config.hpp`.
