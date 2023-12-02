/**
 * @brief Logging Utilities for detecting and selecting logging interfaces.
 *
 * Defines the high-level wrapper for logging data on the Data Gator (DG). Currently
 * this consists of a single function `log_data` which takes the data strings to be
 * logged and logs them to all available logging destinations such as MQTT and the 
 * SD card. 
 *
 * Data destinations are controlled by whether their 'is available' flag is set
 * to `true`. This flag is first set in the user configuration files but can be 
 * overridden if the interface cannot be initialized during setup.
 *
 * @author Garret Wells
 * @file logger.hpp
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

extern int reset_count;

/**
 * Helper function that handles all data logging to MQTT and 
 *  the SD card automatically.
 *
 * IF (there is a WiFi connection) -> log to MQTT broker
 *
 * IF (there is a SD card) -> log to SD card
 *
 * @param[in] topic     The MQTT topic to log
 * @param[in] message   The message to log, likely JSON object
 */
void log_data(std::string topic, std::string message){


    bool wifi_connected = WiFi.status() == WL_CONNECTED;
    
    if(wifi_connected){
        // log to MQTT 
        MQTTMailer instance = MQTTMailer::getInstance();
        instance.mailMessage(&mqtt_client, topic, message);
        Serial.println("\t-> logging to MQTT");
    }

    // logging available set when initializing SDLogger in 
    //  setup
    if(logging_available){
        Serial.println("\t-> logging to SD card");

        std::string time;

        if(absolute_timestamp_available){
            // use absolute time from NTP
            time = tsp->get_timestamp();
            log_to_sd_file(logger, time, topic, message);
            logged_relative_data = false;

        }else{
            // use relative timestamp and offset
            // calculate the offset to use
            int log_offset = cache_retrieve_log_offset();
            Serial.printf("[DEBUG] log offset:%i, reset_count:%i, tsp offset:%i\n", log_offset, reset_count, tsp->get_offset());

            int new_offset;
            // prevent reset_count wrap from breaking
            // relative time keeping
            if(log_offset > reset_count){
                // offset has exceeded MAX_COUNT and wrapped 
                // so make the new offset the old offset + net distance 
                // between log_offset and reset_count
                new_offset = tsp->get_offset() + (MAX_COUNT - log_offset) + reset_count;

            }else{
                // new offset is cached offset + distance between 
                // current reset count and previous marker
                new_offset = tsp->get_offset() + (reset_count - log_offset);

            }

            // parse the date and time from previous time stamp
            //
            std::string date = std::to_string(tsp->get_month()) + "-" + 
                std::to_string(tsp->get_day()) + "-" + 
                std::to_string(tsp->get_year());

            std::string clock_time = std::to_string(tsp->get_hours()) + ":" + 
                std::to_string(tsp->get_minutes()) + ":" + 
                std::to_string(tsp->get_seconds());

            std::string offset = std::to_string(new_offset);

            time = tsb->get_date_time(date, clock_time, offset);
            // save the timestamp with updated offset
            time_stamp_to_cache = time;

            log_to_sd_file(logger, time, topic, message);

            logged_relative_data = true;
        }

        Serial.printf("[DEBUG] logging \'%s\' | \'%s\' | \'%s\'\n", time.c_str(), topic.c_str(), message.c_str());

    }else{
        Serial.println("[ERROR] no SD card connected when logging");
    }
}

#endif
