/**
 * @file pHSensor.hpp
 * @brief      Implements an interface for converting a voltage (analog reading)
 *             into a pH value according to the conversion or calibration for a
 *             specific sensor.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef PHSENSOR_H
#define PHSENSOR_H

/**
 * @brief Interface for pH sensors
 *
 * Defines functional interface for reading from and converting values from 
 * pH sensors.
 */
class pHSensor {
	public:
        /**
         * @brief Convert voltage reading to pH value
         *
         * @return The pH as a double
         */
		virtual double getpH(double voltage);

        /**
         * @brief Get the sensor type as a string, used for constructing MQTT topics
         *
         * @return The sensor manufacturer and type as a string
         */
        virtual std::string getSensorType();

        /**
         * @brief Converts a voltage to a JSON object string for MQTT messages
         *
         * @return A sensor message with voltage as JSON
         */
        virtual std::string toJSON(double voltage);
};

#endif
