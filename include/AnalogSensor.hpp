/**
 * @file AnalogSensor.hpp
 * @author Kevin Wing (wing5640@vandals.uidaho.edu)
 * @brief An abstract base class to be used for all analog sensors. Provides a common method for returning the
 * analog sensor's current ADC reading. Also declares pure virtual methods for getting the sensor's
 * type and returning a JSON string of the sensor's data. These methods must be implemented by the
 * derived classes.
 * @version 0.1
 * @date 2024-04-05
 */
#ifndef ANALOG_SENSOR_HPP
#define ANALOG_SENSOR_HPP

#include <Arduino.h>

class AnalogSensor
{
public:
    // the following commented out sections are potential future refactoring (keep)
    // AnalogSensor() {} // Default Constructor (empty)
    // Constructor with parameters: pin, reference voltage, maximum resolution, and sensor type
    // AnalogSensor(const uint8_t pin,
    //             //  const double refVoltage,
    //              const double maxResolution,
    //              const std::string sensorType)  //mPin(pin),
                                                //  mRefVoltage(refVoltage),
                                                //  mMaxResolution(maxResolution),
                                                //  mSensorType(sensorType)
    // {
    //     // set the pin to be read as an analog input
    //     // pinMode(mPin, INPUT);
    // }                                                    // Constructor
    virtual std::string getSensorType() = 0;             // Returns the name of the sensor
    virtual std::string toJSON(const double voltage) = 0; // Returns a JSON string of the sensor's data
    virtual double calculate(const double voltage) = 0; // Returns the voltage of the sensor

    // virtual void setRefVoltage(const double refVoltage) { mRefVoltage = refVoltage; }             // Sets the reference voltage
    // virtual void setMaxResolution(const double maxResolution) { mMaxResolution = maxResolution; } // Sets the maximum resolution
    // virtual void setSensorType(const std::string sensorType) { mSensorType = sensorType; }        // Sets the sensor type
protected:
    // virtual ~AnalogSensor(){};                                                       // Empty default Constructor
    // virtual uint16_t readADC(const uint16_t resolution) { return analogRead(mPin); } // Returns the ADC value and returns it

    // double mMaxResolution;                                                           // Known maximum resolution (i.e. 1023 for 10-bit ADC, 4095 for 12-bit ADC, etc.)
    std::string mSensorType;                                                         // Name of the sensor
    // uint8_t mPin;                                                                    // Pin the sensor is connected to
};

#endif // DENDRO_SENSOR_HPP
