/**
 * @file BLESensor.hpp
 * @brief This file implements BLE Sensor generic interface.
 *
 * BLESensor is the parent class for all bluetooth low energy sensors which are currently implemented for the 
 * Data Gator firmware architecture.
 *
 * At this time (Fall 2023), only temperature and humidity sensors are supported, but it is possible that other types
 * could be supported, requiring the parameters passed to toJSON(..) to change.
 *
 * @author     Garrett Wells
 * @date       2022
 */
#ifndef BLESENSOR_H
#define BLESENSOR_H

/**
 * @brief Defines a bluetooth low energy (BLE) interface for integrating new sensors into the DG firmware.
 */
class BLESensor {
public:
    std::string name; //!< Name of the sensor... not unique, generally something generic such as brand and model
    std::string mac_address; //!< Mac address of sensor BLE for unique ID 
                             
    /** @brief convert data values to JSON string object 
     *
     * Constructs as JSON formatted string from the data types which are available to be read from the
     * sensor. Defaults to temperature and humidity.
     *
     * @param[in] temp The temperature as a double in Celsius 
     * @param[in] humidity The humidity as a double in relative humidity
     */
	virtual std::string toJSON(double temp, double humidity) = 0;

    /** @brief get the sensor type as a string 
     * 
     * Returns a string which identifies the sensor. Usually contains both the generic
     *  identifier constructed of brand and model, and the unique MAC address.
     */
	virtual std::string getSensorType() = 0;
};

#endif
