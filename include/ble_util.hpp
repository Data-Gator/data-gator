/**
 * @file ble_util.hpp
 * @brief Utilities such as callbacks for parsing bluetooth packets.
 *
 * # Basic Functional Summary
 * The callback `onResult()` receives data from advertising sensors and then checks if that packet
 *  belongs to a known/supported sensor. If so, then the data is parsed and logged using
 *  the logging utils in logger.hpp.
 *
 * # Integration
 * All BLE supported BLE sensors should be integrated with their own class which inherits from 
 *  BLESensor.hpp. The class should also implement a function `<sensor class>.advertisedDeviceIs<sensor type>(dev)`
 *  which parses the data packet and returns true if identified as an instance of that 
 *  sensor type.
 *
 * @author Garrett Wells
 * @date 2023
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
     *
     * @param[in] dev The packet received by the BLE stack. Contains information needed to identify the sensor as well as data.
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
