/**
 * @file GenericPH.hpp
 * @brief This file implements Generic PH which inherits from pHSensor.hpp
 *
 * Defines a sensor interface for one of the myriad unbranded pH sensors
 * which is unbranded.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef GENERICPH_H
#define GENERICPH_H
#include <../../include/pHSensor.hpp>

/**
 * @brief Overrides the interface provided by pHSensor.hpp for a generic analog pH sensor.
 */
class GenericPH: public pHSensor{
	public:

		double getpH(double voltage){
			voltage = voltage*(3/2);
			double pH = ((voltage/MAX_V) * 6) + 3; // on range 3-9 with min value of 3
			return pH;
		}

        /**
         * @brief Get identifier string for debugging.
         *
         * @returns `"generic_pH"` string
         */
		std::string getSensorType(){
			return "generic_pH";
		}

        /**
         * @brief Converts voltage reading to JSON object for debugging and data collection.
         */
       	std::string toJSON(double voltage){
       		return "\"PH_RAW\": " + std::to_string(voltage) + ", \"PH\":" + std::to_string(this->getpH(voltage));
       	}

    private:
    	const double MIN_V = 0;
    	const double MAX_V = 3.33;


};

#endif
