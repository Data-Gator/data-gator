/**
 * @defgroup   BLESENSOR BLE Sensor
 *
 * @brief      This file implements BLE Sensor.
 *
 * @author     Robotnik
 * @date       2022
 * @file BLESensor.hpp
 */
#ifndef BLESENSOR_H
#define BLESENSOR_H

class BLESensor {
public:
    /** @brief sensor name */
    std::string name;
    /** @brief mac address for unique ID */
    std::string mac_address;
    /** @brief convert data values to JSON string object 
     *
     * @param[in] temp The temperature as a double in Celsius 
     * @param[in] humidity The humidity as a double in relative humidity
     */
	virtual std::string toJSON(double temp, double humidity) = 0;
    /** @brief get the sensor type as a string */
	virtual std::string getSensorType() = 0;
};

#endif
