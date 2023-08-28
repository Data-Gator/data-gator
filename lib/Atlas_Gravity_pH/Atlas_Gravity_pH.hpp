/**
 * @defgroup   GENERICPH Generic PH
 *
 * @brief      This file implements Generic PH.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef ATLASGRAVITYPH_H
#define ATLASGRAVITYPH_H
#include <../../include/pHSensor.hpp>

class AtlasGravitypH: public pHSensor{
	public:
        AtlasGravitypH(){};
		double getpH(double voltage){
			double pH = (-5.6548 * voltage) + 15.509;
			return pH;
		}

		std::string getSensorType(){
			return "atlas_gravity_ph";
		}
       	std::string toJSON(double voltage){
       		return "\"PH_RAW\": " + std::to_string(voltage) + ", \"PH\":" + std::to_string(this->getpH(voltage));
       	}

};

#endif
