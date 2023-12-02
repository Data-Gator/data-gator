/**
 * @file VWCSensor.hpp
 * @brief Interface for VWC sensors
 *
 * Defines interface for reading and converting values from 
 * sensor readings.
 *
 * Should be overriden to implement various hardware interfaces.
 *
 *
 * @author     Garrett Wells
 * @date       2022
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
        /**
         * @brief Defines types of soil for which the volumetric water content calculation
         *  needs to be adjusted.
         *
         * These types are defined according to the needs of the 
         *   [Meter Teros10](https://publications.metergroup.com/Manuals/20788_TEROS10_Manual_Web.pdf), though in principle this can be extended to other
         *   sensors or disregarded by sub-classes. Pass to `VWCSensor::getVWC(...)`
         *   to apply to conversion from analog voltage to digital VWC value.
         */
        enum SoilMedia {
            MINERAL_SOIL,   //!< Mineral-based soil. Dirt!
            SOILLESS        //!< Fake soil, potting soil for example.
        };

        /**
         * @brief Convert raw data to volume water/volume soil percentage
         *
         * @param[in] media The type of soil the sensor is going to be used in. This is assumed to be natural dirt by default, but some sensors
         *                      allow this value to be changed to provide better accuracy.
         *
         * @param[in] voltage The raw reading from the sensor.
         *
         * @returns Soil volumetric water content as a double. Values outside of range [0.0, 1.0] indicate sensor error.
         */
		virtual double getVWC(SoilMedia media, double voltage); 

        /**
         * @brief Return brand+sensor model string for topic
         *
         * @returns The sensor type as a string identifier, not unique since most VWC sensors are wired with no unique identifier.
         */
        virtual std::string getSensorType();                    
        
        /**
         * @brief Convert data to JSON key value pairs
         *
         * @param[in] voltage The analog voltage read from the sensor.
         *
         * @returns a JSON object string with both the raw reading (voltage) and the converted 
         * value as a float with valid values on range [0, 1].
         */
        virtual std::string toJSON(double voltage); 
        
};

#endif
