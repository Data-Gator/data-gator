/**
 * @defgroup   TEROS10 Teros 10
 *
 * @brief      This file implements Teros 10.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef TEROS10_H
#define TEROS10_H

#include <Arduino.h>
#include <../../include/VWCSensor.hpp>

class Teros10: public VWCSensor{
private:
	int analog_pin; // pin the sensor is connected to and can be read from
	int pwr_pin;	// pin that must be powered to read from the sensor

public:

	Teros10(int analog_pin, int pwr_pin);
	Teros10(){}
	float readVoltage();					// read just the voltage measured by the sensor
	double getVWC(SoilMedia media);			// read the sensor and return the VWC as a decimal which represents the VWC as a percentage
	double getVWC(SoilMedia media, double voltage); 

	std::string toJSON(double voltage){
		return "\"VWC_RAW\":" + std::to_string(voltage) + ", \"VWC\":" + std::to_string(this->getVWC(MINERAL_SOIL, voltage));
	}

	std::string getSensorType();
	double apparentDialectricPermitivity(); // read dialectric permitivity which can then be converted to VWC using Topp equation
};

#endif
