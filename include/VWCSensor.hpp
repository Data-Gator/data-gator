/**
 * @brief Interface for VWC sensors
 *
 * Defines interface for reading and converting values from 
 * sensor readings.
 *
 * Should be overriden to implement various hardware interfaces.
 *
 * @defgroup   VWC_SENSOR VWC Sensor
 *
 * @brief      This file implements VWC Sensor.
 *
 * @author     Garrett Wells
 * @date       2022
 *
 * @file VWCSensor.hpp
 */
#ifndef VWC_SENSOR_H
#define VWC_SENSOR_H


/**
 * @brief      Interface for converting a voltage measurement from an analog
 *             sensor into a double, as a Volume of water per volume of soil
 *             percentage.
 */
class VWCSensor {

	public:
        enum SoilMedia {MINERAL_SOIL, SOILLESS};

        /**
         * @brief Convert raw data to volume water/volume soil percentage
         */
		virtual double getVWC(SoilMedia media, double voltage); 

        /**
         * @brief Return brand+sensor model string for topic
         */
        virtual std::string getSensorType();                    
        
        /**
         * @brief Convert data to JSON key value pairs
         */
        virtual std::string toJSON(double voltage); 
        
};

#endif
