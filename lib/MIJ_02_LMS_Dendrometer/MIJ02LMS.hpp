/**
 * @file MIJ02LMS.hpp
 * @author Kevin Wing (wing5640@vandals.uidaho.edu)
 * @brief Class Declaration for the MIJ02LMS Dendrometer. Provides a method to read the sensor's value,
 * and a method to get the sensor's name.
 * @version 0.1
 * @date 2024-02-23
 */
#ifndef MIJ02LMS_HPP
#define MIJ02LMS_HPP

// Standard Library Includes
// #include <../../include/dendroSensor.hpp>
#include <../../include/AnalogSensor.hpp>

// since the voltage to the sensor seems to maintain around 3.0V, this value is hardcoded currently
#define VPRE 3.0 // Preheat Voltage
#define VPRE_MV (VPRE * 1000.0) // Preheat Voltage in mV

class MIJ02LMS : public AnalogSensor
{
public:
    MIJ02LMS(); // Default Constructor
    MIJ02LMS(const double maxResolution, const std::string sensorType);
    ~MIJ02LMS(){};                                     // Empty default Destructor
    double getUM(const double voltage);              // Returns the analog value read from the sensor
    std::string getSensorType() override;              // Returns the name of the sensor
    std::string toJSON(const double voltage) override; // Returns a JSON string of the sensor's data
    // double calculatePreheatMV(const double rawPreheatADC); // Returns the preheat voltage of the sensor
    // double calculateMV(const double voltage); // Returns the voltage of the sensor
    virtual double calculate(const double voltage); // Returns the voltage of the sensor
protected:
    // double mRefVoltage; // Known reference voltage
    double mMaxResolution; // Known maximum resolution
    std::string mSensorType; // Name of the sensor
};

#endif // MIJ02LMS_HPP
