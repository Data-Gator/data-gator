/**
 * @file main.cpp
 * @brief Initializes device and calls routines.
 *
 * Uses the Arduino framework to initialize devices and call 
 * routines to collect data and manage power. This file is required for compilation
 * in the Arduino framework. Furthermore it contains implementations of the Arduino
 * required functions `setup()` and `loop()`. 
 *
 * The Arduino model of execution has been adapted to fit a different executional model
 * which prioritizes intermittent, low power data collection. Arduino defaults to running the 
 * code within loop an infinite number of times until the device is stopped or runs out of a resource
 * such as power. 
 *
 * In many precision agriculture scenarios the rate of change of any monitored variable such as
 * temperature likely needs to be polled no more than once a minute. Any more than this
 * degree of resolution is probably a waste of energy. To accomodate this norm, the Data Gator (DG) 
 * firmware is configured to run the code in loop _**once**_ and then shutdown (enter low power mode)
 * to conserve battery life.
 *
 *
 * @brief      This file implements main.
 *
 * @author     Garrett Wells
 * @date       2022
 */
// standard c and arduino framework libraries
#include <Arduino.h>
#include <cstring>

// custom headers
#include "SDReader.hpp"
#include <version.hpp>
#include <pinout.hpp>
#include <config.hpp>
#include <firebeetle_sleep.hpp>
#include <update.cpp>
#include <logging_util.cpp>
#include <mqtt_util.hpp>
#include <setup_util.hpp>
#include <scheduler.hpp>
#include <logger.hpp>
// installed through platformio
#include <SPI.h>

#include <HttpsOTAUpdate.h>
#include <Preferences.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/** Interface for the analog to digital converter.  */
extern Adafruit_ADS1115 ads; //!< Analog to digital converter object (I2C)
/** Interface for the battery fuel gauge. */
extern Adafruit_MAX17048 maxlipo; //!< MAX17048 battery Fuel Gauge
/** External flag indicating whether maxlipo was initialized successfully. */
extern bool maxlipo_attached; //!< Fuel Gauge successfully initialized?
/** External variable holding the number of resets recorded by the system. Stored and loaded from non-volatile storage. */
extern int reset_count; //!< number of resets retrieved for NVS


/** Serial debug enable flag, set in config.h */
const bool USB_DEBUG = DEBUG; //!< USB serial debugging enabled

WiFiClient wifi_client; //!< WiFi stack object
PubSubClient mqtt_client(wifi_client); //!< MQTT client object
/** NVS memory access interface. */
Preferences gator_prefs; //!< NVS memory object

/**
 * @brief Setup initializes sensor interfaces, NVS, wireless protocols, and logging options.
 * 
 * This function initializes all global resources and bus based protocols. A list (non-exhaustive but pretty complete)
 * can be found below:
 *
 *  1. initializes gpio pins,
 *  2. initializes i2c sensors,
 *  3. starts serial debug interface and waits 1 sec (defaults to 115200 baud),
 *  4. initializes the non-volatile memory system (specific to ESP32, adapt for other uC), 
 *  5. initializes wireless connections,
 *  6. initializes logging systems and configures flags based on what logging interfaces are available.
 *
 */
void setup(){

    // initialize pins for output/input and set default state
    setup_gpio();
    // initialize adc and fuel gauge and other i2c bus sensors
    setup_i2c_sensors();
    // start serial by default for printing firmware version
	Serial.begin(115200);
	delay(1000);
	// start non-volatile storage system (NVS)
	init_nvs();
    Serial.printf("FIRMWARE VERSION v%i.%i.%i\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    // connect to WiFi, BLE, etc
    setup_wireless_connections();
    // detect logging options (MQTT, SD card, etc)
    setup_logging();

}


/**
 * @brief Read sensors, log data, and hibernate.
 *
 * Although specified to loop infinitely by the Arduino API, this function does the following in one iteration:
 *  
 *  1. reads sensors, 
 *  2. logs data, 
 *  3. and finishes by putting the system into hibernation to conserve energy.
 *
 */
void loop(){
    // if no logging or wifi, sleep
    if(!logging_available && WiFi.status() != WL_CONNECTED){

        hibernate(65);

    }else{
        if(WiFi.status() == WL_CONNECTED){mqtt_client.loop();}

        if(logging_available) Serial.println("[DEBUG] SD card detected");

        // allow scheduler to run tasks
        Scheduler(reset_count);

        digitalWrite(SD_PWR_EN, LOW);

        if(logged_relative_data){
            // save the current reset count
            //  but don't overwrite the cached timestamp
            cache_log_offset(reset_count);
            cache_timestamp(time_stamp_to_cache);

        }else if(logging_available && absolute_timestamp_available){
            // save the new timestamp to NVS
            cache_timestamp(tsb->get_date_time());
            // reset the log_offset
            cache_log_offset(reset_count);

        }else{
            Serial.println("[DEBUG] no timestamp cached because nothing logged");
        }

        // go to sleep until next WDT reset
        //  or wake from sleep
        if(DEBUG) Serial.println("Sleeping...");

        //deep_sleep(120);
        if(WiFi.status() == WL_CONNECTED){mqtt_client.loop();}
        hibernate(65); // has same effect as watchdog
    }

}

