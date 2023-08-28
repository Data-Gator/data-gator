#include "KKM_K6P.hpp"


/**
 * @brief      Check if the advertised device is a KKMK6P sensor
 *
 * @param[in]  dev   The advertised data received
 *
 * @return     true if the device has a service UUID that is recognized, false otherwise
 */
bool KKMK6P::advertisedDeviceIsK6P(NimBLEAdvertisedDevice* dev){
    if(dev->haveName()){
        this->name = dev->getName();
        //Serial.printf("Name: %s\n", name.c_str());

        // check if the start is KBPro
        if(name.compare(0, 5, "KBPro") == 0){
            //Serial.printf("matches!\n");
            return true;
        }else{
            //Serial.printf("%s is not equal to %s\n", name.substr(0, 5).c_str(), "KBPro"); 
            return false;

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
MQTTMail* KKMK6P::parseAdvertisedData(NimBLEAdvertisedDevice* dev){

	if(dev->haveServiceData()){ // HT, TLM, URL frames

		std::string data_str = dev->getServiceData();
		const char* data_raw = data_str.data();

		// print in reverse order because it is big endian
		if(USB_DEBUG){
			Serial.print("\t");
			for(int i = 0; i < data_str.length(); i++){
				Serial.printf("[%2X]", data_raw[i]);
			}
			Serial.println();
        }

		if(data_raw[0] == 0x21){ // Eddystone TLM
            Serial.println("\t[DEBUG] Eddystone TLM Frame");
			memcpy(&TLM_Frame, data_raw, data_str.length());

			if(USB_DEBUG){
				Serial.println("\t[DEBUG] Eddystone TLM Frame Parsed");
				Serial.printf("\t%s\n", TLM_Frame.toString().c_str());
				Serial.printf("\tMAC: %s\n", dev->getAddress().toString().c_str());
			}

			std::string tlm_json = "\"MAC\": \"" + dev->getAddress().toString() + 
                                    "\", \"HUMIDITY\": " + std::to_string(this->getHumidity()) + 
                                    ", \"TEMP\": " + std::to_string(this->getTemp()) + 
                                    ", \"BATT_VOLTAGE\": " + std::to_string(this->getVoltage()) + 
                                    ", \"SENSOR_NAME\": \"" + this->name + "\"";

			MQTTMail* mail = new MQTTMail(std::string("kkm_k6p/") + dev->getAddress().toString(), tlm_json);
			return mail;
        }


	}
	return NULL;
}

/**
 * @brief      Convert the uint16_t in HT_Frame to a float temperature value in Celsius
 *
 * @return     temperature in celsius as a float
 */
float KKMK6P::getTemp(){
    float tmp = TLM_Frame.temp.upper;
    if(TLM_Frame.temp.lower >= 99){
        tmp += (float)TLM_Frame.temp.lower/1000;
    }else if(TLM_Frame.temp.lower >= 9){
        tmp += (float)TLM_Frame.temp.lower/100;
    }else{
        tmp += (float)TLM_Frame.temp.lower/10;
    }

	return tmp;	
}

/**
 * @brief      Convert uint16_t from HT_Frame to a float relative humidity value
 *
 * @return     relative humidity as a float
 */
float KKMK6P::getHumidity(){
    float tmp = TLM_Frame.humidity.upper;
    if(TLM_Frame.humidity.lower >= 99){
        tmp += (float)TLM_Frame.humidity.lower/1000;
    }else if(TLM_Frame.humidity.lower >= 9){
        tmp += (float)TLM_Frame.humidity.lower/100;
    }else{
        tmp += (float)TLM_Frame.humidity.lower/10;
    }

	return tmp;	
}

/**
 * @brief      Gets the voltage.
 * @return     the voltage in mV as an integer
 */
int KKMK6P::getVoltage(){
	return TLM_Frame.volt;
}

/*
uint32_t KKMK6P::timeUp(){
	return TLM_Frame.tmil;
}
*/


BLEAddress KKMK6P::getMAC(){
	std::string addr = "";
	for(int i = 5; i >= 1; i--){
		addr += HT_Frame.address[i] + ":";
	}
	addr += HT_Frame.address[0];

	return BLEAddress(addr);
}

std::string KKMK6P::getSensorType(){
	return "kkm_k6p";
}

std::string KKMK6P::toJSON(double temp, double humidity){
	return "\"TEMP\": " + std::to_string(temp) + ", \"HUMIDITY\": " + std::to_string(humidity);
}
