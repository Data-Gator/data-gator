/**
 * @file dendroSensor.hpp
 * @author Kevin Wing (wing5640@vandals.uidaho.edu)
 * @brief Base Class to be used for all dendrometer sensors. Provides a method to calculate the diameter of the tree in micrometers,
 * and a method to get the sensor's name. Also provides a method to return a JSON string of the sensor's data.
 * @version 0.1
 * @date 2024-02-28
 */
#ifndef DENDRO_SENSOR_HPP
#define DENDRO_SENSOR_HPP

#include <Arduino.h>

class DendroSensor
{
public:
    virtual double getUM(const double rawADC) = 0;                                                        // Returns the diameter of the tree in micrometers
    virtual std::string getSensorType() = 0;                                                           // Returns the name of the sensor
    virtual std::string toJSON(const double rawADC, const double rawPreheatADC) = 0;                                               // Returns a JSON string of the sensor's data
protected:
    virtual ~DendroSensor(){};                     // Empty default Constructor
    // virtual double calculateMV(const int rawADC) = 0; // Returns the voltage of the sensor
    double mRefVoltage;                               // Known reference voltage
    double mMaxResolution;                            // Known maximum resolution
    std::string mSensorType;                          // Name of the sensor
};

#endif // DENDRO_SENSOR_HPP
