#include "MinewS1.hpp"


/**
 * @brief      Check if the advertised device is a Minew S1 sensor
 *
 * @param[in]  dev   The advertised data received
 *
 * @return     true if the device has a service UUID that is recognized, false otherwise
 */
bool MinewS1::advertisedDeviceIsS1(NimBLEAdvertisedDevice* dev){

	if(dev->haveServiceUUID()){ // check if advertised service is registered with S1

		NimBLEUUID serviceID = dev->getServiceUUID();
		for(int i = 0; i < serviceCount; i++){
			if(serviceID.equals(s1_services[i])){
				return true;
			}
		}

	}

	return false;
}

/**
 * @brief      Retrieve data advertised by the device and save it if of interest.
 *
 * @param[in]  dev    device and data that were broadcast
 * @param[in]  debug  flag noting whether serial debug summary is desired
 */
MQTTMail* MinewS1::parseAdvertisedData(NimBLEAdvertisedDevice* dev){

	if(dev->haveManufacturerData()){ // iBeacon frame
		if(USB_DEBUG) Serial.println("\t[DEBUG] iBeacon Frame Parsed");
		return NULL;

	}else if(dev->haveServiceData()){ // HT, TLM, URL frames

		std::string data_str = dev->getServiceData();
		const char* data_raw = data_str.data();

		// print in reverse order because it is big endian
		if(USB_DEBUG){
			Serial.print("\t");
			for(int i = data_str.length() - 1; i >= 0; i--){
				Serial.printf("[%X]", data_raw[i]);
			}
			Serial.println();
		}

		// identify service and store data
		if(data_raw[0] == 0x10){ // Eddystone URL
			if(USB_DEBUG) Serial.println("\t[DEBUG] Eddystone URL Frame Parsed");

		}else if(data_raw[0] == 0x20){ // Eddystone TLM
			if(data_str.length() != sizeof(TLM_Frame)){
				if(USB_DEBUG){
					Serial.println("size of service data does not match TLM struct size");
				}
				return NULL;
			}

			memcpy(&TLM_Frame, data_raw, data_str.length());

			if(USB_DEBUG){
				Serial.println("\t[DEBUG] Eddystone TLM Frame Parsed");
				Serial.printf("\t%s\n", TLM_Frame.toString().c_str());
				Serial.printf("\tMAC: %s\n", dev->getAddress().toString().c_str());
			}

			//std::string tlm_json = "\"MAC\": \"" + dev->getAddress().toString() + "\", \"VOLTAGE\": " + std::to_string(TLM_Frame.volt) + ", \"TEMP\": " + std::to_string(((float)ENDIAN_CHANGE_U16(TLM_Frame.temp))/256);

			std::string tlm_json = "\"MAC\": \"" + dev->getAddress().toString() + 
                                    "\", \"HUMIDITY\": " + std::to_string(this->getHumidity()) + 
                                    ", \"TEMP\": " + std::to_string(this->getTemp()) + 
                                    ", \"BATT_VOLTAGE\": " + std::to_string(this->getVoltage()) + 
                                    ", \"SENSOR_NAME\": \"Minew S1\"";

			MQTTMail* mail = new MQTTMail(std::string("minew_s1/") + dev->getAddress().toString(), tlm_json);
			return mail;


		}else if(data_raw[0] == 0xa1){ // HT or INFO

			if(data_str.length() == sizeof(HT_Frame)){ // HT
				memcpy(&HT_Frame, data_raw, data_str.length());

				if(USB_DEBUG){ 
					Serial.println("\t[DEBUG] HT Frame Parsed");
					Serial.printf("\t%s\n", HT_Frame.toString().c_str());
					Serial.printf("\tMAC: %s\n", dev->getAddress().toString().c_str());
				}

                //	std::string json = "\"MAC\": \"" + dev->getAddress().toString() + 
                //	                    "\", " + this->toJSON(((float)ENDIAN_CHANGE_U16(HT_Frame.temp))/256, ((float)ENDIAN_CHANGE_U16(HT_Frame.humidity))/256);

                std::string json = "\"MAC\": \"" + dev->getAddress().toString() + 
                                        "\", \"HUMIDITY\": " + std::to_string(this->getHumidity()) + 
                                        ", \"TEMP\": " + std::to_string(this->getTemp()) + 
                                        ", \"BATT_VOLTAGE\": " + std::to_string(this->getVoltage()) + 
                                        ", \"SENSOR_NAME\": \"Minew S1\"";

				//return new MQTTMail(*dev.getAddress().getNative(), HT, std::string("minew_s1/") + dev.getAddress().toString(), data);
				return new MQTTMail(std::string("minew_s1/") + dev->getAddress().toString(), json);


			}else if(data_str.length() == sizeof(INFO_Frame)){ // INFO
				memcpy(&INFO_Frame, data_raw, data_str.length());

				if(USB_DEBUG){ 
					Serial.println("\t[DEBUG] INFO Frame Parsed");
					Serial.printf("\t%s\n", INFO_Frame.toString().c_str());
					Serial.printf("\tMAC: %s\n", dev->getAddress().toString().c_str());
				}
				return NULL;
			}

		}
	}
	return NULL;

}

/**
 * @brief      Convert the uint16_t in HT_Frame to a float temperature value in Celsius
 *
 * @return     temperature in celsius as a float
 */
float MinewS1::getTemp(){
    int16_t signed_ver = ENDIAN_CHANGE_U16(HT_Frame.temp);
    //Serial.printf("\t\tTemp(raw): %i, (end flipped): %i, (flipped + signed): %i\n", HT_Frame.temp, ENDIAN_CHANGE_U16(HT_Frame.temp), signed_ver);
	//return ((float)ENDIAN_CHANGE_U16(HT_Frame.temp))/256;	
    return ((float)signed_ver)/256;
}

/**
 * @brief      Convert uint16_t from HT_Frame to a float relative humidity value
 *
 * @return     relative humidity as a float
 */
float MinewS1::getHumidity(){
	return ((float) ENDIAN_CHANGE_U16(HT_Frame.humidity))/256;
}

/**
 * @brief      Gets the voltage.
 *
 * @return     the voltage in mV as an integer
 */
int MinewS1::getVoltage(){
	return ENDIAN_CHANGE_U16(TLM_Frame.volt);
}

uint32_t MinewS1::timeUp(){
	return TLM_Frame.tmil;
}


BLEAddress MinewS1::getMAC(){
	std::string addr = "";
	for(int i = 5; i >= 1; i--){
		addr += HT_Frame.address[i] + ":";
	}
	addr += HT_Frame.address[0];

	return BLEAddress(addr);
}

std::string MinewS1::getSensorType(){
	return "minew_s1";
}

std::string MinewS1::toJSON(double temp, double humidity){
	return "\"TEMP\": " + std::to_string(temp) + ", \"HUMIDITY\": " + std::to_string(humidity);
}
