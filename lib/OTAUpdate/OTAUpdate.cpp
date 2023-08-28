/**
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

using namespace tinyxml2;
using namespace std;

#define OTA_SERVER "192.168.50.10"

extern const bool USB_DEBUG;

string new_firmware_version_available() {

    //Serial.println("\tchecking available firmware version");
    string fw_version_url = string("http://") + string(OTA_SERVER) + string("/current_version.txt");
    if(USB_DEBUG) Serial.printf("\tchecking new fw version @ %s\n", fw_version_url.c_str());
		
    HTTPClient httpClient;
    httpClient.begin( fw_version_url.c_str() );

    int httpCode = httpClient.GET();

    if ( httpCode == 200 ) {
        string fw_filename = httpClient.getString().c_str();

        if(fw_filename.length() < 10) return "failure";

        if(USB_DEBUG){
            Serial.print("\tgot version string: \"");
            Serial.print(fw_filename.substr(fw_filename.length()-10, 6).c_str());
            Serial.println("\"");
        }

    	int version_major = stoi(fw_filename.substr(fw_filename.length()-9, 1));
    	int version_minor = stoi(fw_filename.substr(fw_filename.length()-7, 1));
        int version_patch = stoi(fw_filename.substr(fw_filename.length()-5, 1));

    	if(USB_DEBUG) Serial.printf("\tv_maj: %d, v_min: %d\n, v_patch: %d\n", version_major, version_minor, version_patch);

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

void update_progress(int cur, int total){
	if(USB_DEBUG) Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_started(){
	if(USB_DEBUG) Serial.println("CALLBACK: HTTP update process started");
}

void update_error(int err) {
    if(USB_DEBUG) Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

/*
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
            /*
            httpUpdate.onEnd(update_finished);
            httpUpdate.onProgress(update_progress);
            httpUpdate.onError(update_error);
            */

            string fw_url = string("http://") + string(OTA_SERVER) + string("/") + file_name;
            if(USB_DEBUG) Serial.println(fw_url.c_str());

            t_httpUpdate_return ret = httpUpdate.update(client, fw_url.c_str());
            // Or:
            //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "/file.bin");

            if(USB_DEBUG){
                switch (ret) {
                  case HTTP_UPDATE_FAILED:
                    if(USB_DEBUG) Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                    break;

                  case HTTP_UPDATE_NO_UPDATES:
                    if(USB_DEBUG) Serial.println("HTTP_UPDATE_NO_UPDATES");
                    break;

                  case HTTP_UPDATE_OK:
                    Serial.println("HTTP_UPDATE_OK");
                    break;
                }
            }
        }
	}
}
