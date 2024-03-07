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
#include <../../include/dendroSensor.hpp>

#define VPRE 3.3 // Preheat Voltage
#define VPRE_MV (VPRE * 1000.0) // Preheat Voltage in mV

class MIJ02LMS : public DendroSensor
{
public:
    MIJ02LMS(const double maxResolution, const std::string sensorType);
    ~MIJ02LMS(){};                                     // Empty default Destructor
    double getUM(const double rawADC) override;              // Returns the analog value read from the sensor
    std::string getSensorType() override;              // Returns the name of the sensor
    std::string toJSON(const double voltage, const double rawPreheatADC) override; // Returns a JSON string of the sensor's data
    // double calculatePreheatMV(const double rawPreheatADC); // Returns the preheat voltage of the sensor
    double calculateMV(const double rawADC); // Returns the voltage of the sensor
protected:
    // double mRefVoltage; // Known reference voltage
    double mMaxResolution; // Known maximum resolution
    std::string mSensorType; // Name of the sensor
};

#endif // MIJ02LMS_HPP
