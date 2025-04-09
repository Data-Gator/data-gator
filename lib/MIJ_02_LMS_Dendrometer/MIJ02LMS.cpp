/**
 * @file MIJ02LMS.cpp
 * @author Kevin Wing (wing5640@vandals.uidaho.edu)
 * @brief Class Implementation for the MIJ02LMS Dendrometer. Provides a method to read the sensor's value,
 * and a method to get the sensor's name. Also provides a method to return a JSON string of the sensor's data.
 * @version 0.1
 * @date 2024-02-23
 */
#include <MIJ02LMS.hpp>

/**
 * @brief Default Constructor
 */
MIJ02LMS::MIJ02LMS() : mMaxResolution(4095.0), mSensorType("mij_02_lms_dendrometer") {}

/**
 * @brief Construct a new MIJ02LMS::MIJ02LMS object
 *
 * @param refVoltage The known reference voltage (e.g. 3.3, 5.0, etc.)
 * @param maxResolution The known maximum resolution of ADC (e.g. 4095.0 for 12-bit)
 * @param sensorType The name of the sensor (e.g. "mij_02_lms_dendrometer")
 */
MIJ02LMS::MIJ02LMS(const double maxResolution, const std::string sensorType) : mMaxResolution(maxResolution), mSensorType(sensorType) {}

/**
 * @brief Returns the diameter of the tree in micrometers. The formula used to calculate the diameter is:
 * dr = 11000. * (Vout / (Vpre) / (1 + sqrt(2))
 * where:
 * dr = radius displacement in micrometers (um)
 * Vout = voltage read from the sensor (mV)
 * Vpre = preheat voltage (mV) : 3300.0mV for 3.3V
 *
 * @param volgate The voltage read from the sensor
 * @return double radius displacment in micrometers (um)
 */
double MIJ02LMS::getUM(const double voltage)
{
    // Convert the voltage to millivolts
    double mV = voltage * 1000.0;
    // return the radius displacement in micrometers
    // dr = 11000 * (Vout / Vpre) / (1 + sqrt(2)
    return 11000.0 * (mV / VPRE_MV) / (1 + M_SQRT2);
}

/**
 * @brief Returns the name of the sensor
 *
 * @return std::string The name of the sensor
 */
std::string MIJ02LMS::getSensorType()
{
    return "mij_02_lms";
}

/**
 * @brief Returns a JSON string of the sensor's data
 *
 * @param rawADC The raw analog value read from the sensor (e.g. 0-4095, 0-1023, etc.)
 * @return std::string A JSON string of the sensor's data
 */
std::string MIJ02LMS::toJSON(const double voltage)
{
    return "\"VOLTAGE\":" + std::to_string(voltage) + ", \"RADIUS\":" + std::to_string(this->calculate(voltage));
}

/**
 * @brief Returns the voltage of the sensor
 *
 * @param voltage The voltage read from the sensor
 * @return double The voltage of the sensor
 */
double MIJ02LMS::calculate(const double voltage)
{
    return getUM(voltage);
}
