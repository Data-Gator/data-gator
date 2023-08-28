/**
 * @brief Utilities for scheduling tasks and saving persistent data.
 *
 * Defines the mechanism for implementing task scheduling for sensors, OTA updates, etc.
 * Timing is measured in "ticks". One tick is approximately 64 seconds or the time that the
 * watchdog timer waits before reseting the device and waking it from hibernation.
 * 
 * Ticks are tracked between power cycles using the non-volatile storage (NVS) system.
 *
 * @author Garrett Wells
 * @file scheduler.hpp
 */

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

//#include <setup_util.hpp>
#include <NimBLEDevice.h>
#include <Adafruit_MAX1704X.h>
#include <OWMAdafruit_ADS1015.h>
#include <ble_util.hpp>
#include <VWCSensor.hpp>
#include <Teros10.hpp>
#include <Atlas_EZO-pH.hpp>
#include <Atlas_Gravity_pH.hpp>

extern bool maxlipo_attached;
extern Adafruit_MAX17048 maxlipo;
extern Adafruit_ADS1115 ads;

int reset_count = -1; // times reset by WDT, one tick roughly equivalent to one minute

/**
 * Counters for when the last time the task was executed.
 *
 * Last reset counter value when reading was taken.
 */
struct planner{
    /** time since analog reading */
	int analog_t0 = -1;	 
    /** time since temperature and humidity sensor reading */
	int ht_t0 = -1;		
    /** time since over the air update check was made */
	int ota_t0 = -1;	
    /** time since the last telemetry message was sent */
	int tlm_t0 = -1;	
}planner;

/**
 * Open NVS, check if it is initialized with data, if not, initialize it.
 */
void init_nvs(){

	// NVS Setup
	gator_prefs.begin("GatorState", RW_MODE);

	if(gator_prefs.isKey("reset_count")){ // if key exists
		reset_count = gator_prefs.getInt("reset_count");
		reset_count++;
		if(DEBUG) Serial.printf("Reset Count = %d\n", reset_count);
		gator_prefs.putInt("reset_count", reset_count);

		planner.analog_t0 = gator_prefs.getInt("analog_t0");
		planner.ht_t0 = gator_prefs.getInt("ht_t0");
		planner.ota_t0 = gator_prefs.getInt("ota_t0");
		planner.tlm_t0 = gator_prefs.getInt("tlm_t0");

	}else{ // create the keys
		reset_count = planner.analog_t0 = planner.ht_t0 = planner.ota_t0 = 1;
		if(DEBUG) Serial.printf("Reset Count = %d\n", reset_count);
		gator_prefs.putInt("reset_count", reset_count);	
		gator_prefs.putInt("analog_t0", planner.analog_t0);
		gator_prefs.putInt("ht_t0", planner.ht_t0);
		gator_prefs.putInt("ota_t0", planner.ota_t0);
		gator_prefs.putInt("tlm_t0", planner.tlm_t0);

	}

}

/**
 * Check if a task will run this time, NVS must be initialized first!
 *
 * @param[in] reset_count The number of resets recorded.
 */
bool task_is_scheduled(int reset_count){
    bool run_vwc = reset_count - planner.analog_t0 >= VWC_FREQ;
    bool run_ht = reset_count - planner.ht_t0 >= HT_FREQ;
    bool run_ota_update = reset_count - planner.ota_t0 >= OTA_FREQ;
    bool run_tlm = reset_count - planner.tlm_t0 >= TLM_FREQ;

    if( run_vwc || run_ht || run_ota_update || run_tlm){
        // start WIFI
        return true;
    }else return false;

}

/**
 * @brief      Reads temperature and humidity sensors via BLE and then sends complete data to the database
 */
void ReadHT(){
	if(DEBUG) Serial.println("[HT]");
	NimBLEScan* scanner = NimBLEDevice::getScan();
	scanner->setAdvertisedDeviceCallbacks(new ScanCallbacks());
	scanner->setActiveScan(true); 	// retrieve results from all scanned devices
	scanner->setInterval(100);		// time between scans??
	scanner->setWindow(99);		// less than or equal to setInterval, is how long to scan for
	//scanner->start(5, true);

	int scanTime = 10; // in seconds

	BLEScanResults res = scanner->start(scanTime, false);
	scanner->clearResults();
}

/**
 * @brief      Reads all wired sensors attached to the aggregator
 */
void ReadWired(){

	if(DEBUG) Serial.println("[VWC/WIRED SENSORS] queueing data");

    // turn on power to sensors
	digitalWrite(PWR_EN, HIGH);
	delay(10000);

    // initialize sensor readers
	VWCSensor* vwc_converter = new Teros10();
	pHSensor* pH_converter = new AtlasGravitypH();
    AtlasEZOpH* ezopH_converter = new AtlasEZOpH();

	MQTTMailer instance = MQTTMailer::getInstance();
    // get the MAC address
	std::string mac_str(WiFi.macAddress().c_str());

    
	int raw_analog[4];
	double voltage[4];

    // read voltage at analog ports
    // SHALLOW
	raw_analog[0] = ads.readADC_SingleEnded(1);
	voltage[0] = raw_analog[0] * 0.0001875;
    // MIDDLE
	raw_analog[1] = ads.readADC_SingleEnded(2);
	voltage[1] = raw_analog[1] * 0.0001875;
    // DEEP
	raw_analog[2] = ads.readADC_SingleEnded(3);
	voltage[2] = raw_analog[2] * 0.0001875;
    // ANALOG/pH
	raw_analog[3] = ads.readADC_SingleEnded(0);
	voltage[3] = raw_analog[3] * 0.0001875;

    // read I2C sensors
    char pH_normal[32];
    char pH_shallow[32];
    char pH_middle[32];
    char pH_deep[32];
    // NORMAL
    if(ezopH_converter->sensor_at_address(EZO_I2C_ADDR)){
        strcpy(pH_normal, ezopH_converter->getpH_str(EZO_I2C_ADDR));
        ezopH_converter->clear_pH_str();
        // build pH mqtt message
        std::string topic = ezopH_converter->getSensorType() + "/pH/normal/" + mac_str;
        //std::string msg = "{\"MAC\": \"" + mac_str + "\"," + pH_converter->toJSON(voltage[3]) + "}";
        std::string msg = "{\"MAC\": \"" + mac_str + "\", \"PH\":" + pH_normal + "}";
        //instance.mailMessage(&mqtt_client, topic, msg);
        log_data(topic, msg);
    
    }else if(DEBUG){
        Serial.println("\tno pH at addr 99");
    }
    // SHALLOW
    if(ezopH_converter->sensor_at_address(EZO_I2C_SHALLOW_ADDR)){
        strcpy(pH_shallow, ezopH_converter->getpH_str(EZO_I2C_SHALLOW_ADDR));
        ezopH_converter->clear_pH_str();
        // build pH mqtt message
        std::string topic = ezopH_converter->getSensorType() + "/pH/shallow/" + mac_str;
        //std::string msg = "{\"MAC\": \"" + mac_str + "\"," + pH_converter->toJSON(voltage[3]) + "}";
        std::string msg = "{\"MAC\": \"" + mac_str + "\", \"PH\":" + pH_shallow + "}";
        //instance.mailMessage(&mqtt_client, topic, msg);
        log_data(topic, msg);
    
    }else if(DEBUG){
        Serial.println("\tno pH at addr shallow");
    }
    // MIDDLE
    if(ezopH_converter->sensor_at_address(EZO_I2C_MIDDLE_ADDR)){
        strcpy(pH_middle, ezopH_converter->getpH_str(EZO_I2C_MIDDLE_ADDR));
        ezopH_converter->clear_pH_str();
        std::string topic = ezopH_converter->getSensorType() + "/pH/middle/" + mac_str;
        //std::string msg = "{\"MAC\": \"" + mac_str + "\"," + pH_converter->toJSON(voltage[3]) + "}";
        std::string msg = "{\"MAC\": \"" + mac_str + "\", \"PH\":" + pH_middle + "}";
        //instance.mailMessage(&mqtt_client, topic, msg);
        log_data(topic, msg);
    
    }else if(DEBUG){
        Serial.println("\tno pH at addr middle");
    }
    // DEEP
    if(ezopH_converter->sensor_at_address(EZO_I2C_DEEP_ADDR)){
        strcpy(pH_deep, ezopH_converter->getpH_str(EZO_I2C_DEEP_ADDR));
        ezopH_converter->clear_pH_str();
        std::string topic = ezopH_converter->getSensorType() + "/pH/deep/" + mac_str;
        //std::string msg = "{\"MAC\": \"" + mac_str + "\"," + pH_converter->toJSON(voltage[3]) + "}";
        std::string msg = "{\"MAC\": \"" + mac_str + "\", \"PH\":" + pH_deep + "}";
        //instance.mailMessage(&mqtt_client, topic, msg);
        log_data(topic, msg);
    
    }else if(DEBUG){
        Serial.println("\tno pH at addr deep");
    }

    free(pH_converter);
	
    // turn off power to sensors
	digitalWrite(PWR_EN, LOW);

	// build VWC mqtt message
	std::string brand = vwc_converter->getSensorType();

	if(WiFi.status() == WL_CONNECTED && !mqtt_client.connected()){
		if(DEBUG) Serial.println("\t-> not connected");
		instance.reconnect(mqtt_client);
	}

	for(int i = 0; i < 3; i++){

		std::string depth = "";
		switch(i){
			case 0:
				depth = "shallow";
				break;
			case 1:
				depth = "middle";
				break;
			case 2:
				depth = "deep";
				break;
		}
		
		std::string topic = brand + "/" + std::to_string(i) + std::string("_") + depth + std::string("/") + mac_str;
		std::string msg = "{\"MAC\": \"" + mac_str + "\", \"DEPTH\": \"" + depth + "\", " + vwc_converter->toJSON(voltage[i]) + "}";
		//instance.mailMessage(&mqtt_client, topic, msg);
        log_data(topic, msg);
	}
	free(vwc_converter);
}

/**
 * @brief      Checks and updates from new code published at HTTPS address
 */
void OTAUpdate(){
	if(USB_DEBUG) Serial.println("[OTA] ran");
	attempt_update();
}

/**
 * @brief      Pat the watchdog by raising watchdog done pin high, and then setting it low
 */
void PatWDT(){
	if(USB_DEBUG) Serial.println("[WDT] ran");
	digitalWrite(DONE, HIGH);
	vTaskDelay(pdMS_TO_TICKS(10));
	digitalWrite(DONE, LOW);
}

/**
 * Send a telemetry message to the MQTT broker.
 */
void SendTLM(){
	if(DEBUG){
        Serial.println("[TLM] queueing data");
    }

    digitalWrite(PWR_EN, HIGH);

	MQTTMailer instance = MQTTMailer::getInstance();

	if(WiFi.status() == WL_CONNECTED && !mqtt_client.connected()){
		if(USB_DEBUG) Serial.println("\t-> not connected");
		instance.reconnect(mqtt_client);
	}

	std::string fw_version = "V" + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + "." + std::to_string(VERSION_PATCH);
	std::string topic = "datagator/tlm/" + std::string(WiFi.macAddress().c_str());
	std::string msg = "{ \"MAC\": \"" + std::string(WiFi.macAddress().c_str()) + 
                        "\", \"FIRMWARE_VERSION\": \"" + fw_version + 
                        "\", \"RSSI\": " + std::to_string(WiFi.RSSI()) + 
                        ", \"BSSID\": \"" + WiFi.BSSIDstr().c_str() + "\"";

    if(maxlipo_attached){
        msg = msg + ", \"BATT_VOLTAGE\": " + std::to_string(maxlipo.cellVoltage()) +
                    ", \"BATT_PERCENTAGE\": " + std::to_string(maxlipo.cellPercent()) + "}";
    }else{
        msg = msg + ", \"BATT_VOLTAGE\": -1" +
                    ", \"BATT_PERCENTAGE\": -1}";
    }
    
    digitalWrite(PWR_EN, LOW);
	//instance.mailMessage(&mqtt_client, topic, msg);
    log_data(topic, msg);
}


/**
 * @brief      Perform a state transition based on the number of reset counts
 *
 * @param[in]  reset_count The number of resets that have been performed in this epoch
 */
void Scheduler(int reset_count){

    // bounds checking -> error msgs
	if(reset_count < 0 || reset_count > MAX_COUNT || planner.analog_t0 > reset_count){
		if(DEBUG) Serial.println("[ERROR] over ran max reset count without reseting count, check if variable is being reset or if tasks are not completing");
		//Serial.printf("\tPlanner { read analog [%d], read ht [%d], published MQTT [%d] }\n", planner.readAnalog, planner.readHT, planner.publishedMQTT);
		reset_count = planner.analog_t0 = planner.ht_t0 = planner.ota_t0 = planner.tlm_t0 = 1;
		gator_prefs.putInt("reset_count", reset_count);	
		gator_prefs.putInt("analog_t0", planner.analog_t0);
		gator_prefs.putInt("ht_t0", planner.ht_t0);
		gator_prefs.putInt("ota_t0", planner.ota_t0);
		gator_prefs.putInt("tlm_t0", planner.tlm_t0);
	}

    bool run_vwc = reset_count - planner.analog_t0 >= VWC_FREQ;
    bool run_ht = reset_count - planner.ht_t0 >= HT_FREQ;
    bool run_ota_update = reset_count - planner.ota_t0 >= OTA_FREQ;
    bool run_tlm = reset_count - planner.tlm_t0 >= TLM_FREQ;

	if(run_vwc){
		ReadWired();
		planner.analog_t0 = reset_count;
		gator_prefs.putInt("analog_t0", planner.analog_t0);
        mqtt_client.loop();
	}

	if(run_ht){
		ReadHT();
		planner.ht_t0 = reset_count;
		gator_prefs.putInt("ht_t0", planner.ht_t0);
		// Publish happens in callback function
        mqtt_client.loop();
	}

	if(run_ota_update){
		planner.ota_t0 = reset_count;
		gator_prefs.putInt("ota_t0", planner.ota_t0);
		OTAUpdate();
        mqtt_client.loop();
	}

	if(run_tlm){
		SendTLM();
		planner.tlm_t0 = reset_count;
		gator_prefs.putInt("tlm_t0", planner.tlm_t0);
        mqtt_client.loop();
	}

}

/**
 * Clear all tasks so that none are scheduled to run
 *
 * @param[in] reset_count The number of reset counts to have elapsed.
 */
void SchedulerClearAll(int reset_count){
    gator_prefs.putInt("ota_t0", reset_count);
    gator_prefs.putInt("tlm_t0", reset_count);
    gator_prefs.putInt("analog_t0", reset_count);
    gator_prefs.putInt("ht_t0", reset_count);
}

#endif
