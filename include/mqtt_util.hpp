/**
 * @brief Utilities for receiving and sending MQTT commands.
 *
 * Utilities and interface for receiving MQTT messages from the broker. Also used to 
 * request data logged to the uSD card from the device. 
 *
 * 1. defines a basic callback to 
 * attach to the MQTT client object to handle received messages.
 * 2. defines a function to check and take proper action on commands
 *  received via MQTT.
 *
 *
 *
 * @author Garrett Wells
 * @file mqtt_util.hpp
 */
#include <ArduinoJson.h>
#include <string>
#include "SDReader.hpp"

/**
 * @brief Process a command execute it.
 *
 * Checks the command, if the command matches one of the known commands,
 * then the message is processed. The message contains details about 
 * how the command should be carried out.
 *
 * @param[in] command The command as a string.
 * @param[in] message The body/message sent with the command.
 */
void process_command(string command, string message){

    if(command == "get_time_range"){
        // convert message to json structure
        StaticJsonDocument<256> doc;
        deserializeJson(doc, message);

        int page_size;
        JsonArray topic_filter_ja = {};
        string time_range;

        if(!doc.containsKey("page_size")){
            if(USB_DEBUG) Serial.println("[ERROR] MQTT command \'get_time_range\' missing key \'page_size\' so using default size of 20");
            page_size = 20;

        }else{
            page_size = doc["page_size"]; 
        }

        if(!doc.containsKey("time_range")){
            if(USB_DEBUG) Serial.println("[ERROR] MQTT command \'get_time_range\' missing key \'time_range\' so quitting");
            return;

        }else{
            time_range = doc["time_range"].as<string>();
        }

        if(!doc.containsKey("topic_filter")){
            if(USB_DEBUG) Serial.println("[ERROR] MQTT command \'get_time_range\' missing key \'topic_filter\' so using default \"\"");
            topic_filter_ja.add("");
        }else{
            topic_filter_ja = doc["topic_filter"];
        }

        // empty topic filter vector
        vector<string> topic_filter_v = {};

        // convert topic filter jason array to vector
        for(JsonVariant n : topic_filter_ja){
            topic_filter_v.push_back(n.as<string>());
        }

        // parse time range string for epochs
        int ampersand_pos = time_range.find("&");
        string epoch = time_range.substr(0, ampersand_pos);
        string terminus = time_range.substr(ampersand_pos+1);

        TimeStamp* ep = NULL;
        TimeStamp* term = NULL;

        // convert strings to TimeStamp objects
        if(epoch.find("T") != string::npos){
            ep = new TimeStamp(epoch);
        }else{
            ep = new TimeStamp(stoi(epoch));
        }

        if(terminus.find("T") != string::npos){
            term = new TimeStamp(terminus);
        }else{
            term = new TimeStamp(stoi(terminus));
        }



        Serial.println("[DEBUG] pulling data range");
        /*
        if(USB_DEBUG){
            Serial.print("[DEBUG] process command has ");
            double perc = ESP.getFreeHeap();
            Serial.print(perc);
            Serial.println(" free bytes");
        }
        */
        // read data from files and upload via MQTT
        SDReader sdr;
        sdr.read_entry_range_from_files(
                *ep, 
                *term, 
                topic_filter_v,
                page_size);

    }else{

        if(USB_DEBUG) {
            Serial.print("[DEBUG] unkown command \'");
            Serial.print(command.c_str());
            Serial.println("\'");
        }

    }

    if(USB_DEBUG) Serial.println("[DEBUG] finished processing MQTT command");
}


/**
 * @brief      Called when MQTT message is passed to the device by the broker.
 *
 * Prints command and message to serial interface,
 * then parses the MQTT packet and processes the command. 
 *
 * Commands are defined as a anything published to the 
 * `datagator/cmd/#` topic tree. The `#` symbol is an MQTT regex
 * glob symbol that matches all MAC addresses. It tells the DG to 
 * subscribe to all commands published to ALL DGs. 
 *
 * The command is only processed in `process_command(...)` if the 
 * MAC address in the topic, such as `datagator/cmd/AC:45...` 
 * matches the MAC of this DG.
 *
 * @param[in] topic    The topic the message was published on
 * @param[in] message  The message in the MQTT packet
 * @param[in] length   The length of the message
 */
void callback(char* topic, byte* message, unsigned int length){
    std::string msg_str;
    for(int i = 0; i < length; i++){
        msg_str += (char)message[i];
    }

    Serial.printf("[MQTT] Received \'%s\'|\'%s\'\n", topic, msg_str.c_str());

    // TODO: check if something needs to be done when receiving MQTT packets
    // parse command
    string topic_s = topic;
    int mac_id_pos = topic_s.find("/", 15);
    string command = topic_s.substr(14, mac_id_pos - 14);
   
    // parse mac id
    string mac_id = topic_s.substr(mac_id_pos + 1);

    // check if MAC is match before processing command
    if(WiFi.macAddress().c_str() == mac_id){
        process_command(command, msg_str);
    }

    
}
