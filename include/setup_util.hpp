/**
 * @brief Utilities to facilitate setup of devices and interfaces.
 * @author Garrett Wells
 * @file setup_util.hpp
 */

#ifndef SETUP_UTIL_H
#define SETUP_UTIL_H

// Preferences Access Modes
#define RO_MODE true
#define RW_MODE false

#include <scheduler.hpp>

#include <OWMAdafruit_ADS1015.h>
#include <Adafruit_MAX1704X.h>

#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEAddress.h>
#include <NimBLEAdvertisedDevice.h>

// ADC
Adafruit_ADS1115 ads;
// Fuel Gauge
Adafruit_MAX17048 maxlipo;
bool maxlipo_attached = true;
// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

extern int reset_count;

/**
 * @brief Initialize pins as input/output and set default state (HIGH/LOW)
 */
void setup_gpio(){

    // power switch pin
    pinMode(PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, LOW);

    // sd card power switch pin
    pinMode(SD_PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, LOW);

    // WDT pins
	pinMode(DONE, OUTPUT);
	digitalWrite(DONE, LOW);


}

/**
 * @brief Initialize the analog to digital converter.
 */
void setup_adc(){

	// setup ADS1x1x	
	ads.begin();
	//ads.setGain(GAIN_TWO);
    
}

/**
 * @brief Initialize and set the state variable for the fuel gauge.
 */
void setup_fuel_gauge(){

    // setup Fuel Gauge
    if(!maxlipo.begin()) maxlipo_attached = false;

}

/**
 * @brief Initialize ALL i2c bus dependent sensors.
 */
void setup_i2c_sensors(){
    
    // enable power to sensors for setup
    digitalWrite(PWR_EN, HIGH);

    setup_adc();
    setup_fuel_gauge();

    // disable power to sensors
    digitalWrite(PWR_EN, LOW);
}

/**
 * @brief Configure mqtt client settings.
 */
void setup_mqtt_connection(){

    // configue mqtt client connection
    if(USB_DEBUG){
        Serial.print("[DEBUG] bytes free =  ");
        double perc = ESP.getFreeHeap();
        Serial.println(perc);
    }
    mqtt_client.setBufferSize(30000);
    mqtt_client.setKeepAlive(120);
    mqtt_client.setServer(MQTT_BROKER_ADDR, MQTT_PORT);
    mqtt_client.setCallback(callback);
    if(USB_DEBUG){
        Serial.print("[DEBUG] bytes free after setting heap size =  ");
        double perc = ESP.getFreeHeap();
        Serial.println(perc);
    }

    // connect to MQTT
    string mqtt_client_id = string("dg_") + WiFi.macAddress().c_str();
    // connect as persistent/durable client
    bool mqtt_client_connected = mqtt_client.connect(mqtt_client_id.c_str(), NULL, NULL, NULL, 0, false, NULL, false);

    if(mqtt_client_connected){
        std::string unique_topic = "datagator/cmd/#";
        Serial.print("Subscribing to ");
        Serial.println(unique_topic.c_str());
        mqtt_client.subscribe(unique_topic.c_str(), 1);
        /*
           if(USB_DEBUG){
           Serial.print("[DEBUG] bytes free after connecting & subscribing =  ");
           double perc = ESP.getFreeHeap();
           Serial.println(perc);
           }
           */
        if(USB_DEBUG) Serial.println("connected to MQTT client!");
    }else{
        if(USB_DEBUG) Serial.println("[WARNING] error with MQTT connection");
        delay(2000);
    }

}

/**
 * @brief Initialize BLE stack.
 */
void setup_ble(){

        // initialize BLE
        NimBLEDevice::init("datagator");
        /*
        if(USB_DEBUG){
            Serial.print("[DEBUG] bytes free after BLE init =  ");
            double perc = ESP.getFreeHeap();
            Serial.println(perc);
        }
        */

}

/**
 * @brief Connect to WiFi, set flags, timeout, etc.
 */
void setup_wifi_connection(){

    WiFi.begin(NETWORK, PSSWD);
    //wifi_client.setInsecure();

    // default version print
    Serial.printf("Gator MAC address: %s\n", WiFi.macAddress().c_str());
    Serial.printf("FIRMWARE VERSION v%i.%i.%i\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    Serial.printf("WiFi connecting %s, %s\n", NETWORK, PSSWD);

    long int t0 = millis();
    long int time_elapsed = 0;
    while(WiFi.status() != WL_CONNECTED){
        time_elapsed = millis() - t0;
        delay(1000);
        Serial.print(".");

        if (time_elapsed > (WIFI_TIMEOUT)){
            Serial.println("\n[WARNING] WIFI_TIMEOUT");
            break;
            // collect data, but store locally
        }    
    }

    // can start time client w/o wifi
    //  but can't update
    timeClient.begin();
    tsb = new TimeStampBuilder(&timeClient);

    if(DEBUG && WiFi.status() == WL_CONNECTED){
        Serial.println("\nCONNECTED");
        Serial.println();
        Serial.print("Gator connected @ ");
        Serial.println(WiFi.localIP());
        Serial.printf("Gator MAC address: %s\n", WiFi.macAddress().c_str());
    }

}

/**
 * @brief Setup all wireless interfaces including WiFi, BLE.
 */
void setup_wireless_connections(){

    if (task_is_scheduled(reset_count)){
	    setup_wifi_connection(); 
        setup_mqtt_connection();
        setup_ble();
    }

}

/**
 * @brief Initialize logging and get timestamp
 * from network, if available, for logging.
 */
void setup_logging(){

    // initialized data logger and set state flag
    //  NOTE: power needs to be turned on for initialization
    //          turning off power to card while initialized 
    //          seems to terminate the connection
    digitalWrite(SD_PWR_EN, HIGH);
    if(WiFi.status() == WL_CONNECTED){
        // update with NTP
        timeClient.update();
        absolute_timestamp_available = true;
    }

    logging_available = init_data_logger();

}

#endif
