/**
 * @file MinewS1.hpp
 * @brief Implementation of the Minew S1 bluetooth sensor family. Extends BLESensor.hpp.
 *
 * @author Garrett Wells
 * @date 2022
 */
#ifndef MINEW_S1_H
#define MINEW_S1_H

#include <Arduino.h>
#include <sstream>
#include <string.h>

#include <../../include/BLESensor.hpp>
#include <NimBLEAdvertisedDevice.h>
#include <MQTTMailer.hpp>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
#define ENDIAN_CHANGE_U32(x) ((((x)&0xFF000000)>>24) + (((x)&0x00FF0000)>>8)) + ((((x)&0xFF00)<<8) + (((x)&0xFF)<<24))

extern const bool USB_DEBUG;

/**
 * @brief Singleton used to identify and parse BLE transmissions from MinewS1 BLE Temperature and Humidity sensors. Inherits from BLESensor.hpp.
 */
class MinewS1: public BLESensor{
public:
	MinewS1(){}

	static MinewS1& getInstance(){
		static MinewS1 instance;
		return instance;
	}
	
	bool advertisedDeviceIsS1(NimBLEAdvertisedDevice* dev); // true if the data in this payload is from S1
	MQTTMail* parseAdvertisedData(NimBLEAdvertisedDevice* dev); // get the data from a scanned device

    /**
     * @brief Read the temperature from the sensor data packet
     */
	float getTemp();

    /**
     * @brief Read the humidity from the sensor data packet
     */
	float getHumidity();

    /**
     * @brief Read the sensor's battery voltage from the sensor data packet
     */
	int getVoltage();

    /**
     * @brief Read the time the sensor has been running from the sensor data packet
     */
	uint32_t timeUp();

    /**
     * @brief Read the sensor BLE MAC address from the sensor data packet
     */
	BLEAddress getMAC();

    /**
     * @brief Convert temperature and humidity values to a JSON object string
     */
	std::string toJSON(double temp, double humidity); 	// export all known information to json string object
                                                        //
    /**
     * @brief Get the string identifier for the sensor brand and model
     */
	std::string getSensorType();


private:
	int serviceCount = 3; // how many services are stored in the list below, needed for iterating over the list in checks
	NimBLEUUID s1_services[3] = {NimBLEUUID("0000ffe1-0000-1000-8000-00805f9b34fb"), 		// HT
								NimBLEUUID("0000fff1-0000-1000-8000-00805f9b34fb"),	// Eddystone URL
								NimBLEUUID("0000feaa-0000-1000-8000-00805f9b34fb")};	// Eddystone TLM

    /**
     * @brief HT data struct
     */
    struct { // HT data
		uint8_t id = 0x00;			// payload id
		uint16_t unknown = 0x0000;	// 2 bytes of useless data
		int16_t temp = 0x0000;		// temperature in Celsius as a float
		uint16_t humidity = 0x0000;	// relative humidity as a float
		uint8_t address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// S1 MAC address

		// convert to a string of information
		std::string toString(){

			std::stringstream ss;
			ss << ((float)ENDIAN_CHANGE_U16(temp))/256 << " C, " 
				<< ((float)ENDIAN_CHANGE_U16(humidity))/256 << "%RH";

			return ss.str();
			
		}

    } __attribute__((packed)) HT_Frame;

    /**
     * @brief Info struct
     */
    struct { // INFO data
		uint8_t id = 0x00;			// payload id
		uint16_t unknown = 0x0000;	// 2 bytes of useless data
		uint8_t address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// S1 MAC address
		char name[2] = {0x00, 0x00};

		// convert to a string of information
		std::string toString(){

			auto size = 18;
			char *res = (char*)malloc(size);
			snprintf(res, size, "%02x:%02x:%02x:%02x:%02x:%02x", address[5], address[4], address[3], address[2], address[1], address[0]);
			std::string ret(res);
			free(res);
			std::stringstream ss;
			ss << "MAC: " << ret << ", "
				<< "Name: "<< name[0] << name[1];

			return ss.str();
		}

    } __attribute__((packed)) INFO_Frame;

    /**
     * @brief Eddystone struct
     */
    struct { // Eddystone Telemetry data
    	uint8_t frameType = 0x00; 	// eddystone frame specifier
		uint8_t version = 0x00;		// version...
		uint16_t volt = 0x0000;		// voltage of sensor battery in mV
		int16_t temp = 0x0000;		// temperature reading as float in Celsius
		uint32_t advCount = 0x00000000;	// how many advertisements have been broadcast
		uint32_t tmil = 0x00000000;		// time up, days, hours, minutes, you get the idea

		std::string toString(){

			std::stringstream ss;
			ss << "V" << (int)version << ".0, "
				<< ENDIAN_CHANGE_U16(volt) << "mV, "
				<< ((float)ENDIAN_CHANGE_U16(temp))/256 << " C, "
				<< advCount << ", "
				<< ENDIAN_CHANGE_U32(tmil) << " seconds";

			return ss.str();

		}

    } __attribute__((packed)) TLM_Frame;

    /**
     * @brief iBeacon struct
     */
    struct { // iBeacon UUID, Major, & Minor data
    	uint8_t manufacturerID[6];

		std::string toString(){
			return "";
		}

    } __attribute__((packed)) iBeacon_Frame;

};

#endif
