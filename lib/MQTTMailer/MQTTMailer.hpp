/**
 * @defgroup   MQTTMAILER MQTT Mailer
 *
 * @brief      This file implements MQTT Mailer which allows the user to send MQTT packets of data to an MQTT broker.
 *
 * @author     Garrett Wells
 * @date       2022
 */

#ifndef MQTTMAILER_H
#define MQTTMAILER_H

#include "PubSubClient.h" /*MQTT Interface*/
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <BLEAddress.h>
#include <esp_gap_ble_api.h>

extern const bool USB_DEBUG;

/*
 * Packet design for saving data between transmissions
 */
enum PacketType_t {
	    	   INVALID, // data is garbage, all values should be either 0 or INT_MAX
			   FUDGED, 	// data has been intentionally set to a "believable value", but don't trust it, it's still garbage
			   VWC,		// volumetric water data 
			   PH,		// same as above, but fourth sensor field is the soil PH reading
			   HT,      // temperature and/or humidity data
			   TLM_BLE,	// telemetry data from BLE sensor ()
			   TLM 		// telemetry data for the aggregator		
			  };

/**
 * @brief      An object that stores data from which a topic and message can be extracted for publishing to an MQTT broker
 */
class MQTTMail{
public:
	MQTTMail(){}
	MQTTMail(std::string topic, std::string message){
		this->topic = topic;
		this->message = message;
	}
	
	std::string getMessage(); // output MQTT compatible representation of data
	std::string getTopic();	 // get the topic string connected to this message
                             //
    std::string to_string(){ return getTopic() + getMessage(); }

private:
	std::string topic;					// MQTT topic to publish to
	std::string message;

	PacketType_t packet_type = INVALID; // what information is stored in this packet
};

/**
 * @brief      Converts MQTTMail objects into viable MQTT messages so that they can be published
 */
class MQTTMailer {
public:
	static MQTTMailer getInstance(){
		static MQTTMailer instance = MQTTMailer();
		return instance;
	}

	void mailMessage(
            PubSubClient* mqtt_client, 
            std::string topic, 
            std::string message,
            bool serial_debug=true);

	void reconnect(PubSubClient mqtt_client);
	std::string buildMessage(std::string* elements, int num_elements);

private:
	MQTTMailer(){}	
};

#endif
