/**
 * @brief Utilities for logging data.
 *
 * # Summary
 * Define the helper functions/utilities which have been written for 
 *  logging data on the Data Gators.
 *
 * Emphasis is placed on making this as simple to use as possible. Unfortunately it is likely
 *  that the implementation is rather difficult to follow due to the number of edge
 *  cases covered. These include: 
 *
 *  * multiple logging interfaces(SD, WiFi, etc)
 *  * cases where network connection is not available to obtain timestamp for data 
 *
 *  ### Key Functionality
 *  Key functionality includes functions for:
 *
 *  1. caching and retreiving data from NVS 
 *      * timestamps 
 *      * log offsets (calculated from # of resets)
 *  2. constructing file names for logging 
 *  3. logging to SD card file
 *
 * @author Garrett Wells
 * @file logging_util.cpp
 */
#include <Preferences.h>
#include <NTPClient.h>
#include <TimeStamp.hpp>    // functions for formatting time stamps
#include <SDLogger.hpp>     
#include <SDReader.hpp>

// interface to NVM access
extern Preferences gator_prefs; //!< Reference to non-volatile-storage on ESP32
bool logging_available = false; //!< is some logging interface available?
/**< Flag showing that some logging interface is available. May be uSD card or MQTT. */
bool absolute_timestamp_available = false; //!< is an exact/accurate timestamp available?
/**< Flag set during initialization, indicates that WiFi connection to NTP for update was obtained. */
bool logged_relative_data = false; //!< relatively accurate timestamp used, last exact timestamp + # of minutes/resets elapsed         
/**< Flag set to indicate that no network connection was available for NTP update to get precise timestamp. */

std::string time_stamp_to_cache = ""; //!< the formatted timestamp to write to non-volatile memory for next reset
/**< 
 * The timestamp, last exact timestamp + offset to write to non-volatile storage. 
 * This timestamp will be retrieved on next reset and replaced if network connection to NTP available. If NTP not
 * available, then the offset is incremented.
 * */

// SDLogger 
SDLogger* logger = NULL;        //!< reads and writes to files
// TimeStamp
TimeStampBuilder* tsb = NULL;   //!< builds timestamp strings
TimeStampParser* tsp = NULL;    //!< parses timestamp strings

// uSD Utilities
//bool is_sd_attached(void);                      // check if there is an sd card connected

// TimeStamp Utilities
std::string cache_retrieve_timestamp();         // return the last timestamp as a string
void cache_timestamp(std::string timestamp);    // save the current timestamp to NVM
                                                //
// Logging Utilities
bool init_data_logger();                        // perform all initialization that MUST happen before logging
std::string get_log_filename(TimeStampParser*);                 // build the log filename from timestamp
void log_to_sd_file(std::string filename, 
        std::string time, 
        std::string topic, 
        std::string message); // log an mqtt message to the sd card

/**
 * @brief Initialize logging interfaces.
 *
 * Perform all initialization necessary before logging any data with 
 *  log_message to the uSD card.
 *
 *  @return bool true if successfully initialized, false for:
 *                      * no SD card detected
 *                      * some other potential error
 */
bool init_data_logger(){
    Serial.println("[DEBUG] initializing new SDLogger");
    // initialize SDLogger
    logger = new SDLogger();

    // check if the uSD card is connected
    if(!logger->initialize_sd_card()){
        return false;
    }     

    // retrieve latest timestamp
    std::string ts = cache_retrieve_timestamp();
    std::string fn = "";
    Serial.printf("[DEBUG] retrieved cached timestamp -> \'%s\'\n", ts.c_str());
    if(ts != "" && !absolute_timestamp_available){
        // 1. there was a cached timestamp
        // 2. no absolute timestamp from NTP available
        // therefore,
        //  use cached timestamp
        tsp = new TimeStampParser(ts);
        // generate log filename
        fn = get_log_filename(tsp);

    }else if(absolute_timestamp_available){
        // use absolute timestamp to initialize
        tsp = new TimeStampParser(tsb->get_date_time());
        Serial.printf("[DEBUG] using absolute timestamp -> \'%s\'\n", tsp->get_timestamp().c_str());
        fn = get_log_filename(tsp);

    }else{
        // 1. no cached timestamp
        // 2. no absolute timestamp
        // therefore,
        //  set to default log file name
        fn = "/log";
        // set timestamp to default value
        tsp = new TimeStampParser("00-00-00T00:00:00+0");
        Serial.printf("[DEBUG] using default timestamp -> \'%s\'\n", tsp->get_timestamp().c_str());
    }

    // ready to rock!
    logger->set_filename(fn + ".csv");

    return true;
}

/**
 * Generate a file name based on the current time stamp. Should only be 
 *  called if TimeStampBuilder has been updated.
 *
 *  @param[in] tsp TimeStampParser which defines parser for reading the current time stamp from a string.
 *
 *  @returns a constructed filename `log_<month>-<day>-<year>`
 */
std::string get_log_filename(TimeStampParser* tsp){
    if(tsp == NULL) return "";

    std::string filename = "/log_" + std::to_string(tsp->get_month()) + 
        "-" + std::to_string(tsp->get_day()) + 
        "-" + std::to_string(tsp->get_year());

    return filename;
}

/**
 * Cache/update the last known absolute timestamp from the NTPClient
 *  to non-volatile memory
 *
 *  @param[in] timestamp A timestamp string following the syntax of TimeStampBuilder
 */
void cache_timestamp(std::string timestamp){
    // cache string 
    gator_prefs.putString("timestamp", timestamp.c_str());
}

/**
 * Save the reset count.
 *
 * @param[in] log_offset The offset in ticks.
 */
void cache_log_offset(int log_offset){
    gator_prefs.putInt("log_offset", log_offset);
}

/**
 * Retrieve time stamp from non-volatile memory
 *  so that it can be used for log file access.
 *
 *  @return The string value stored in non-volatile memory,
 *                              should be expected to be of format produced by 
 *                              TimeStampBuilder
 *
 */
std::string cache_retrieve_timestamp(){
    if(!gator_prefs.isKey("timestamp")){
        // return empty string, because it doesn't
        //  exist 
        Serial.println("[WARNING] key \'timestamp\' doesn't exist");
        gator_prefs.putString("timestamp",  "00-00-00T00:00:00+0");
        return "";
    }

    return gator_prefs.getString("timestamp", "").c_str();
}

/**
 * Retrieves the reset count.
 *
 * @return Number of resets/times the device as booted.
 */
int cache_retrieve_log_offset(void){
    if(!gator_prefs.isKey("log_offset")){
        Serial.println("[WARNING] key \'log_offset\' doesn't exist");
        gator_prefs.putInt("log_offset", 0);
        return 0;
    }

    return gator_prefs.getInt("log_offset", 0);
}


/**
 * Write an MQTT message to a file on the SD card. Requires a time stamp and filename.
 *
 * @param [in] filename The string file name to log the topic and message to.
 * @param [in] time string time, formatted according to TimeStampBuilder.
 * @param [in] topic The topic, according to MQTT format.
 * @param [in] message The message which contains data. Often JSON object.
 */
void log_to_sd_file(std::string filename, std::string time, std::string topic, std::string message){
    std::string log_fn = filename;
    if(DEBUG){
        Serial.printf("[SDLogger] logging to \'%s\n\'", log_fn);
    }

    logger = new SDLogger(log_fn);
    
    // Write headers if file doesn't already exist
    if(!logger->exists(filename)){

        std::vector<std::string> header_fields;
        header_fields.push_back("TIME");
        header_fields.push_back("MQTT TOPIC");
        header_fields.push_back("MQTT MESSAGE");

        logger->write_header(header_fields);

    }

    // generate timestamp
    std::string ts = tsb->get_date_time();

    // log as absolute, trust user to have 
    //  generated the timestamp they wanted
    logger->log_absolute_mqtt(time, topic, message);

    //logger->close_card();
}

/**
 * Write an MQTT message to a file on the SD card. Requires a time stamp and filename.
 *
 * @param[in] l The SDLogger instance with all filename and separator
 *                  information pre-set
 *
 * @param[in] time The time hr:min:sec+offset that will be written to the 
 *                  log file
 *
 * @param[in] topic  The mqtt topic the data should have been logged to
 *
 * @param[in] message  The mqtt message containing the data we want to save
 *
 */
void log_to_sd_file(SDLogger* l, std::string time, std::string topic, std::string message){
    /*
    if(DEBUG){
        Serial.printf("[SDLogger] logging to \'%s\n\'", log_fn);
    }
    */

    if(!l->exists()){
        std::vector<std::string> header_fields;
        header_fields.push_back("TIME");
        header_fields.push_back("MQTT TOPIC");
        header_fields.push_back("MQTT MESSAGE");

        l->write_header(header_fields);

    }

    // log as absolute, trust user to have 
    //  generated the timestamp they wanted
    l->log_absolute_mqtt(time, topic, message);
}
