/**
 * @file KKM_K6P.hpp
 * @brief Contains the definition of the KKM K6P BLE sensor, which is based on the BLESensor.hpp interface.
 * 
 * @author Garrett Wells
 * @date 2022
 */
#ifndef KKMK6P_H
#define KKMK6P_H 

#include <Arduino.h>
#include <sstream>
#include <string.h>

#include <../../include/BLESensor.hpp>
#include <NimBLEAdvertisedDevice.h>
#include <MQTTMailer.hpp>

//!< Converts endianess for 16 bit integers
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8)) 
//!< Converts endianess for 32 bit integers
#define ENDIAN_CHANGE_U32(x) ((((x)&0xFF000000)>>24) + (((x)&0x00FF0000)>>8)) + ((((x)&0xFF00)<<8) + (((x)&0xFF)<<24))

extern const bool USB_DEBUG;

/**
 * @brief Defines structure for floating point numbers in dual 8 bit upper/lower format.
 *
 * Uses the toString member to convert the upper and lower 8 bit values to a string
 * of format `<upper>.<lower>` to represent a double.
 *
 * For example:
 *
 * ```cpp
 * // note this is pseudo code!
 * ufloat88.upper = 2
 * ufloat88.lower = 5
 *
 * str a = ufloat88.toString() // value of a = "2.5"
 * ```
 *
 */
typedef struct ufloat88 {
    int8_t upper = 0x00;    //!<  the value to the left of the decimal point
    uint8_t lower = 0x00;   //!< the value to the right of the decimal point

    /**
     * @brief Convert struct members upper and lower to a string double value
     *
     * @returns A string value of form `<upper_int>.<lower_int>`.
     */
    std::string toString(){
        std::stringstream ss;

        ss << (int)upper << "." << (int)lower;
        return ss.str();
    }

} __attribute__((packed)) ufloat88;

/**
 * @brief Singleton used to identify and parse BLE transmissions from KKMK6P BLE Temperature and Humidity sensors.
 */
class KKMK6P: public BLESensor{
public:
	KKMK6P(){}

	static KKMK6P& getInstance(){
		static KKMK6P instance;
		return instance;
	}
	
	bool advertisedDeviceIsK6P(NimBLEAdvertisedDevice*); // true if the data in this payload is from S1
	MQTTMail* parseAdvertisedData(NimBLEAdvertisedDevice* dev); // get the data from a scanned device

	float getTemp();
	float getHumidity();
	int getVoltage();
	uint32_t timeUp();
	BLEAddress getMAC();
	std::string toJSON(double temp, double humidity); 	// export all known information to json string object
	std::string getSensorType();


private:
	int serviceCount = 3; // how many services are stored in the list below, needed for iterating over the list in checks
	NimBLEUUID s1_services[3] = {NimBLEUUID("0000ffe1-0000-1000-8000-00805f9b34fb"), 		// HT
								NimBLEUUID("0000fff1-0000-1000-8000-00805f9b34fb"),	// Eddystone URL
								NimBLEUUID("0000feaa-0000-1000-8000-00805f9b34fb")};	// Eddystone TLM

    struct { // HT data
		uint8_t id = 0x00;			// payload id
		uint16_t unknown = 0x0000;	// 2 bytes of useless data
		uint16_t temp = 0x0000;		// temperature in Celsius as a float
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

    struct { // Eddystone Telemetry data
    	uint8_t frameType = 0x00; 	// eddystone frame specifier
		uint8_t version = 0x00;		// version...
        uint8_t sensor_mask = 0x00; // sensor mask
		uint16_t volt = 0x0000;		// voltage of sensor battery in mV
		ufloat88 temp;		// temperature reading as float in Celsius
		ufloat88 humidity;		// temperature reading as float in Celsius
		ufloat88 accX;		// temperature reading as float in Celsius
		ufloat88 accY;		// temperature reading as float in Celsius
		ufloat88 accZ;		// temperature reading as float in Celsius

		std::string toString(){

			std::stringstream ss;
			ss << "V" << (int)version << ".0, "
				<< volt << "mV, "
				<< temp.toString() << " C, "
				<< humidity.toString() << "%, "
				<< accX.toString() << "mg X, "
                << accY.toString() << "mg Y, "
                << accZ.toString() << "mg Z, ";

			return ss.str();

		}

    } __attribute__((packed)) TLM_Frame;

    struct { // iBeacon UUID, Major, & Minor data
    	uint8_t manufacturerID[6];

		std::string toString(){
			return "";
		}

    } __attribute__((packed)) iBeacon_Frame;

};

#endif
