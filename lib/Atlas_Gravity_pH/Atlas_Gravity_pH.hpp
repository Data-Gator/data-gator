/**
 * @file Atlas_Gravity_pH.hpp
 * @brief This file implements an analog pH sensor which inherits from pHSensor.hpp.
 *
 * The Atlas Gravity pH sensor is a budget, but decently accurate sensor which can be used
 * with solids or liquid pH detection.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef ATLASGRAVITYPH_H
#define ATLASGRAVITYPH_H
#include <../../include/pHSensor.hpp>

/**
 * @brief Defineds an analog pH sensor interface for the Atlas Gravity pH.
 */
class AtlasGravitypH: public pHSensor{
	public:
        //!< Constructor for object, no initialization here.
        AtlasGravitypH(){};

        /**
         * @brief Converts analog voltage reading to a double pH value.
         *
         * Applies a linear equation to convert between voltage and pH for the 
         * given sensor. Requires sensor calibration to be reliable and accurate. 
         * Calibration requires following the manufacturer guide and adjusting the 
         * two values in the linear equation in this method to produce accurate
         * pH readings.
         *
         * @param[in] voltage A raw analog reading in volts.
         *
         * @returns A double pH value on range [0.001, 14.000]
         */
		double getpH(double voltage){
			double pH = (-5.6548 * voltage) + 15.509;
			return pH;
		}

        /**
         * @brief Get the sensor brand and model `atlas_gravity_ph` for debugging.
         *
         * @returns `"atlas_gravity_ph"`
         */
		std::string getSensorType(){
			return "atlas_gravity_ph";
		}

        /**
         * @brief Converts a voltage reading to JSON string object.
         *
         * @returns A JSON string object with `PH_RAW` (voltage) and `PH` (pH) fields. 
         */
       	std::string toJSON(double voltage){
       		return "\"PH_RAW\": " + std::to_string(voltage) + ", \"PH\":" + std::to_string(this->getpH(voltage));
       	}

};

#endif
