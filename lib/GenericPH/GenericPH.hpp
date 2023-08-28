/**
 * @defgroup   GENERICPH Generic PH
 *
 * @brief      This file implements Generic PH.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef GENERICPH_H
#define GENERICPH_H
#include <../../include/pHSensor.hpp>

class GenericPH: public pHSensor{
	public:
		double getpH(double voltage){
			voltage = voltage*(3/2);
			double pH = ((voltage/MAX_V) * 6) + 3; // on range 3-9 with min value of 3
			return pH;
		}

		std::string getSensorType(){
			return "generic_pH";
		}
       	std::string toJSON(double voltage){
       		return "\"PH_RAW\": " + std::to_string(voltage) + ", \"PH\":" + std::to_string(this->getpH(voltage));
       	}

    private:
    	const double MIN_V = 0;
    	const double MAX_V = 3.33;


};

#endif
