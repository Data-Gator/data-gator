/**
 * @file main.cpp
 * @brief Initializes device and calls routines.
 *
 * Uses the Arduino framework to initialize devices and call 
 * routines to collect data and manage power.
 *
 * @defgroup   MAIN main
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
extern Adafruit_ADS1115 ads;        
/** Interface for the battery fuel gauge. */
extern Adafruit_MAX17048 maxlipo;
extern bool maxlipo_attached;
extern int reset_count;


/** Serial debug enable flag, set in config.h */
const bool USB_DEBUG = DEBUG; 

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
/** NVS memory access interface. */
Preferences gator_prefs;

/**
 * @brief Setup initializes sensor interfaces, NVS, wireless protocols, and logging options.
 * 
 * This is a more detailed description.
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
 * Although specified to loop infinitely, this function reads sensors, logs data, and finishes by putting
 * the system into hibernation to conserve energy.
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

/**
 * @mainpage Project Description
 *
 * \section intro_sec Introduction
 * This is the documentation for the Data Gator firmware. It may be used as a detailed reference for extending and 
 * improving firmware for the system.
 */
