/**
 * @file update.cpp
 * @brief Defines utilities for managing firmware updates, using Over-The-Air updates.
 *
 * @defgroup   UPDATE update
 *
 * @brief      This file implements OTA update
 *
 * @author     Garrett Wells, adapted from HTTPUpdate library example code
 * @date       2022
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <tinyxml2.h>
#include <MQTTMailer.hpp>

#include <MQTTMailer.hpp>

using namespace tinyxml2;
using namespace std;

#define OTA_SERVER "192.168.50.10"

extern const bool USB_DEBUG;
extern PubSubClient mqtt_client;

/**
 * @brief Check if server has different firmware version.
 *
 * Accesses the server using the IP address defined by OTA_SERVER. If the server
 * hosted version of doesn't match then the firmware file name is retrieved and
 * the file downloaded from the server.
 *
 * Status messages are logged to serial and the MQTT broker if possible.
 */
string new_firmware_version_available() {

    //Serial.println("\tchecking available firmware version");
    string fw_version_url = string("http://") + string(OTA_SERVER) + string("/current_version.txt");
    if(USB_DEBUG) Serial.printf("\tchecking new fw version @ %s\n", fw_version_url.c_str());
        
    HTTPClient httpClient;
    httpClient.begin( fw_version_url.c_str() );

    int httpCode = httpClient.GET();

    if ( httpCode == 200 ) {
        string fw_filename = httpClient.getString().c_str();
        string fw_version = fw_filename.substr(fw_filename.length()-10, 6).c_str();

        if(fw_filename.length() < 10) return "failure";

        if(USB_DEBUG){
            Serial.print("\tgot version string: \"");
            Serial.print(fw_version.c_str());
            Serial.println("\"");
            Serial.printf("\tfrom: \'%s\'\n", fw_filename.c_str());
        }

        int version_major = stoi(fw_filename.substr(fw_filename.length()-9, 1));
        int version_minor = stoi(fw_filename.substr(fw_filename.length()-7, 1));
        int version_patch = stoi(fw_filename.substr(fw_filename.length()-5, 1));

        MQTTMailer instance = MQTTMailer::getInstance();
        std::string topic = "datagator/ota_status/" + std::string(WiFi.macAddress().c_str());
        std::string msg = "{\"STATUS_MSG\": \"version\", \"SERVER_FW_VERSION\": \"" + fw_version + 
                "\", \"DEVICE_FW_VERSION\": \"v" + to_string(VERSION_MAJOR) + 
                "." + to_string(VERSION_MINOR) + 
                "." + to_string(VERSION_PATCH) + 
                "\"}";
        instance.mailMessage(&mqtt_client, topic, msg);
    

        if(USB_DEBUG) Serial.printf("\tv_maj: %d, v_min: %d, v_patch: %d\n", version_major, version_minor, version_patch);

        // if version doesn't match, update
        if(version_minor != VERSION_MINOR || version_major != VERSION_MAJOR || version_patch != VERSION_PATCH){
            return fw_filename;
        }else{
            return "";
        }


    }else{
        if(USB_DEBUG){
            Serial.print("\thttp connection failed with code ");
            Serial.print(httpCode);
            Serial.print(": ");
            Serial.println(httpClient.errorToString(httpCode));
        }
        return "";
    }
}

/**
 * @brief Prints update process progress to the serial interface.
 *
 * Prints number of bytes downloaded out of the total file size to serial interface, 
 * but only if the USB_DEBUG flag is set.
 *
 * @param cur bytes downloaded so far
 * @param total total file size in bytes
 */
void update_progress(int cur, int total){
	if(USB_DEBUG) Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

/**
 * @brief Send status message to the MQTT broker that the update process started.
 *
 * Notifies MQTT broker and prints to the serial interface that the device has
 * begun updating from the server hosted file.
 */
void update_started(){
	if(USB_DEBUG) Serial.println("CALLBACK: HTTP update process started");
    MQTTMailer instance = MQTTMailer::getInstance();
    std::string topic = "datagator/ota_status/" + std::string(WiFi.macAddress().c_str());
    std::string msg = "{\"STATUS_MSG\": \"started\"}";
    instance.mailMessage(&mqtt_client, topic, msg);

}

/**
 * @brief Log update process finished.
 *
 * Notifies the MQTT broker and prints the update finished message to the serial
 * line.
 */
void update_finished(){
	if(USB_DEBUG) Serial.println("update finished");
    MQTTMailer instance = MQTTMailer::getInstance();
    std::string topic = "datagator/ota_status/" + std::string(WiFi.macAddress().c_str());
    std::string msg = "{\"STATUS_MSG\": \"finished\"}";
    instance.mailMessage(&mqtt_client, topic, msg);

}


/**
 * @brief Log update process errored out.
 *
 * Notifies the MQTT broker and prints the update error message to the serial
 * line. The message on the serial port will contain the HTTP error code. Logging error
 * code is not included in MQTT message since most likely failure is connection.
 */
void update_error(int err) {
    if(USB_DEBUG) Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
	MQTTMailer instance = MQTTMailer::getInstance();
    std::string topic = "datagator/ota/" + std::string(WiFi.macAddress().c_str());
    std::string msg = "{\"STATUS_MSG\": \"error\"}";
    instance.mailMessage(&mqtt_client, topic, msg);
}

/*
 * @brief Attempts OTA firmware update from server. 
 *
 * Check current firmware version on server, if newer then update.
 * 		Example from HTTPUpdate library
 */
void attempt_update() {
	std::string file_name = new_firmware_version_available();

	if(file_name != ""){

		for(int i = 0; i < 3; i++){
            // wait for WiFi connection
            if(USB_DEBUG) Serial.println("starting update attempt");

            WiFiClient client;
            if(USB_DEBUG) Serial.println("client created");
            httpUpdate.onStart(update_started);
            httpUpdate.onEnd(update_finished);
            httpUpdate.onProgress(update_progress);
            httpUpdate.onError(update_error);

            string fw_url = string("http://") + string(OTA_SERVER) + string("/") + file_name;
            if(USB_DEBUG) Serial.println(fw_url.c_str());

            t_httpUpdate_return ret = httpUpdate.update(client, fw_url.c_str());
        }
	}
}
