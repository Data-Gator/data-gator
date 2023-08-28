/**
 * @brief Utilities such as callbacks for parsing bluetooth packets.
 *
 * @author Garrett Wells
 * @file ble_util.hpp
 */

#ifndef BLE_UTIL_HPP
#define BLE_UTIL_HPP

#include <NimBLEAdvertisedDevice.h>
#include <BLESensor.hpp>
#include <MinewS1.hpp>
#include <KKM_K6P.hpp>
#include <logger.hpp>

/**
 * @brief Callbacks for BLE packets
 *
 * Provides a set of callbacks to handle identification, parsing and other stages of 
 * handling BLE devices and packets.
 */
class ScanCallbacks: public NimBLEAdvertisedDeviceCallbacks {

	/**
	 * @brief What to do when a device has been picked up by the scan
     * 
     * Checks device advertisement for known sensor IDs. If recognized, then the packet is 
     * parsed.
	 */	
	void onResult(NimBLEAdvertisedDevice* dev){
        //Serial.printf("Advertised Device: %s\n", dev.toString().c_str());
		MinewS1 s1_interpreter = MinewS1::getInstance();
		bool isS1 = s1_interpreter.advertisedDeviceIsS1(dev);

        KKMK6P k6p = KKMK6P::getInstance();
        bool isK6P = k6p.advertisedDeviceIsK6P(dev);

        if(isK6P){
          // parse data
            MQTTMail* mail_ptr = k6p.parseAdvertisedData(dev);
            if(USB_DEBUG) Serial.println("\t-> sent BLE mail to publisher");

            MQTTMailer instance = MQTTMailer::getInstance();

            if(WiFi.status() == WL_CONNECTED && !mqtt_client.connected()){
                if(USB_DEBUG) Serial.println("\t-> not connected");
                instance.reconnect(mqtt_client);
            }

            std::string msg = "{" + mail_ptr->getMessage() + ", \"GATOR_MAC\": \"" + WiFi.macAddress().c_str() + "\"}";
            //instance.mailMessage(&mqtt_client, mail_ptr->getTopic(), msg);
            log_data(mail_ptr->getTopic(), msg);
            delete(mail_ptr);
            mail_ptr = NULL;
        


        }

		if(isS1){
			MQTTMail* mail_ptr = s1_interpreter.parseAdvertisedData(dev); // print the advertised data after interpretation
			if(mail_ptr != NULL){
				if(USB_DEBUG) Serial.println("\t-> sent BLE mail to publisher");
				MQTTMailer instance = MQTTMailer::getInstance();

				if(WiFi.status() == WL_CONNECTED && !mqtt_client.connected()){
					if(USB_DEBUG) Serial.println("\t-> not connected");
					instance.reconnect(mqtt_client);
				}

				std::string msg = "{" + mail_ptr->getMessage() + ", \"GATOR_MAC\": \"" + WiFi.macAddress().c_str() + "\"}";
				//instance.mailMessage(&mqtt_client, mail_ptr->getTopic(), msg);
                log_data(mail_ptr->getTopic(), msg);
                delete(mail_ptr);
				mail_ptr = NULL;
			}
		}	
	}
};

#endif
