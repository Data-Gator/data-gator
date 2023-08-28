#include "MQTTMailer.hpp"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
#define ENDIAN_CHANGE_U32(x) ((((x)&0xFF000000)>>24) + (((x)&0x00FF0000)>>8)) + ((((x)&0xFF00)<<8) + (((x)&0xFF)<<24))

/**
 * @brief      Convert the data fields to an MQTT message string which can be published
 *
 * @return     String message for publishing, should be properly formatted as a JSON object
 */
std::string MQTTMail::getMessage(){
	return this->message;
}

/**
 * @brief      The topic/destination this MQTT object should be published to
 *
 * @return     String topic to publish to.
 */
std::string MQTTMail::getTopic(){
    return this->topic;
}


/**
 * @brief      Publish an MQTT message to a topic hosted by a specific broker.
 *
 * @param[in]  topic    The topic/destination for the message
 * @param[in]  message  The message in string form
 */
void MQTTMailer::mailMessage(
        PubSubClient* mqtt_client, 
        std::string topic, 
        std::string message,
        bool serial_debug){
    bool success = mqtt_client->publish(topic.c_str(), message.c_str());
    if(!success){
       Serial.printf("\tfailed to send %s | %s", topic.c_str(), message.c_str());
    }
    if(USB_DEBUG && serial_debug) Serial.printf("\t-> sent \'%s\' | \'%s\'\n", topic.c_str(), message.c_str());
}

/**
 * @brief      Attempt to reconnect to MQTT broker every 5 seconds until successful
 *
 * @param[in]  mqtt_client  The mqtt client
 */
void MQTTMailer::reconnect(PubSubClient mqtt_client){

    while(!mqtt_client.connected()){
        if(USB_DEBUG) Serial.println("Attempting MQTT connection...");
        if(/*mqtt_client.connect("aggregator", "vineyard", "R0b0tslab")*/mqtt_client.connect("aggregator")){
            if(USB_DEBUG)Serial.println("connected");
            // TODO: subscribe to topics here
            //mqtt_client.subscribe("topic");
        }else{
            if(USB_DEBUG){
                if(USB_DEBUG){
                    Serial.print("\tconnection failed. rc = ");
                    Serial.println(mqtt_client.state());
                }
            }
            delay(5000);
        }
    }

    if(USB_DEBUG) Serial.println("MQTT Connection successful...");

}

/**
 * @brief      Builds a message.
 *
 * @param      elements      The elements
 * @param[in]  num_elements  The number elements
 *
 * @return     The message.
 */
std::string MQTTMailer::buildMessage(std::string* elements, int num_elements){
    if(USB_DEBUG && elements == NULL){
        Serial.println("[ERROR] cannot build message from NULL reference");
        return "";
    }

    std::string final_msg = "{";

    for(int i = 0; i < num_elements; i++){
        if(i == 0){
            final_msg = final_msg + elements[i];

        }else if(i > 0){
            final_msg = final_msg + ", " + elements[i];

        }
    }

    final_msg = final_msg + "}";

    return final_msg;
}
