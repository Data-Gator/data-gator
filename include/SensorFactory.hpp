#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include <Arduino.h>

#include "config.hpp"
#include "AnalogSensor.hpp"
#include "Teros10.hpp"
#include "MIJ02LMS.hpp"
#include <vector>

typedef enum SensorType {
    UNASSIGNED_TYPE,
    TEROS10_TYPE,
    MIJ02LMS_TYPE
} SensorType;

/**
 * @brief Class used to instantiate a factory to configure sensors based on configuration
 *        found in config.hpp
 * 
 */
class SensorFactory {
public:
    const std::vector<std::shared_ptr<AnalogSensor>> &getAnalogSensors()
    {
        return this->analogSensors;
    };

    /**
     * @brief Initiate sensor ports and create relavent sensor objects for each port
     * 
     */
    void begin()
    {
        // read the sensor types from the config file
        sensorPorts.push_back((SensorType)ANALOG_PORT_0);
        sensorPorts.push_back((SensorType)ANALOG_PORT_1);
        sensorPorts.push_back((SensorType)ANALOG_PORT_2);
        sensorPorts.push_back((SensorType)ANALOG_PORT_3);

        // create the sensor objects
        createAnalogSensors();
    }

protected:
    /**
     * @brief Check the sensor configuration and create relevant sensor type for
     * the corresponding port
     * 
     */
    void createAnalogSensors()
    {
        // std::vector<std::shared_ptr<AnalogSensor>> tempSensors;
        for (int i = 0; i < 4; i++)
        {
            switch (sensorPorts[i])
            {
            case UNASSIGNED_TYPE:
                // no sensor
                // Serial.println("No sensor");
                analogSensors.push_back(nullptr);
                break;
            case TEROS10_TYPE:
                // Serial.println("Teros10 sensor found");
                analogSensors.push_back(std::make_shared<Teros10>());
                break;
            case MIJ02LMS_TYPE:
                // Serial.println("MIJ02LMS sensor");
                analogSensors.push_back(std::make_shared<MIJ02LMS>());
                break;
            default:
                // unexpected sensor value
                // Serial.println("Unexpected sensor value");
                analogSensors.push_back(nullptr);
                break;
            }
        }
    };

    std::vector<std::shared_ptr<AnalogSensor>> analogSensors; // will be list of sensors
    std::vector<SensorType> sensorPorts; // will be list of ports
};

#endif // SENSOR_FACTORY_HPP