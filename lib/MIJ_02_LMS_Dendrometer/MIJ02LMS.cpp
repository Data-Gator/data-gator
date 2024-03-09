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
 * @brief Construct a new MIJ02LMS::MIJ02LMS object
 *
 * @param refVoltage The known reference voltage (e.g. 3.3, 5.0, etc.)
 * @param maxResolution The known maximum resolution of ADC (e.g. 4095.0 for 12-bit)
 * @param sensorType The name of the sensor (e.g. "mij_02_lms_dendrometer")
 */
MIJ02LMS::MIJ02LMS(const double maxResolution, const std::string sensorType) : mMaxResolution(maxResolution), mSensorType(sensorType) {}

/**
 * @brief Returns the diameter of the tree in micrometers. The formula used to calculate the diameter is:
 * dr = (11000 * (Vout / (Vpre)) / (1 + sqrt(2)))
 * where:
 * dr = diameter in micrometers (um)
 * Vout = voltage read from the sensor (mV)
 * Vpre = preheat voltage (mV) : 3300.0mV for 3.3V
 *
 * @param rawADC The raw analog value read from the sensor (e.g. 0-4095, 0-1023, etc.)
 * @return double The diameter of the tree in micrometers (um)
 */
double MIJ02LMS::getUM(const double voltage)
{
    // Vpre = Vin * 1000.0: convert volts to mV (e.g. 3300.0mV for 3.3V)
    // dr = (11000 * (Vout / (Vpre)) / (1 + sqrt(2)))
    // return 11000.0 * (this->calculateMV(rawADC) / 3300.0) / (1 + M_SQRT2);
    return 11000.0 * ((voltage * 1000.0) / VPRE_MV) / (1 + M_SQRT2);
}

/**
 * @brief Returns the name of the sensor
 *
 * @return std::string The name of the sensor
 */
std::string MIJ02LMS::getSensorType()
{
    return "mij_02_lms_dendrometer";
}

/**
 * @brief Returns a JSON string of the sensor's data
 *
 * @param rawADC The raw analog value read from the sensor (e.g. 0-4095, 0-1023, etc.)
 * @return std::string A JSON string of the sensor's data
 */
std::string MIJ02LMS::toJSON(const double voltage)
{
    return "\"DENDRO_RAW\": " + std::to_string(voltage) + ", \"uM\":" + std::to_string(this->getUM(voltage));
}

// /**
//  * @brief Returns the voltage of the sensor in millivolts (mV)
//  *
//  * @param rawADC The raw analog value read from the sensor (e.g. 0-4095 (12-bits), 0-1023 (10-bits), etc.)
//  * @return double The voltage of the sensor in millivolts (mV)
//  */
// double MIJ02LMS::calculateMV(const double voltage)
// {
//     // calculate the voltage
//     // double voltage = voltage * (3.3 / mMaxResolution);
//     // Serial.println("Voltage: " + String(voltage));
//     double voltageMV = voltage * 1000.0;
//     // Serial.println("VoltageMV: " + String(voltageMV));
//     return voltageMV;
//     // return ((rawADC / mMaxResolution) * mRefVoltage) * 1000.0;
// }

/**
 * @brief Returns the preheat voltage of the sensor in millivolts (mV)
 *
 * @param rawADC The raw analog value read from the sensor (e.g. 0-4095 (12-bits), 0-1023 (10-bits), etc.)
 * @return double The preheat voltage of the sensor in millivolts (mV)
 */
// double MIJ02LMS::calculatePreheatMV(const double rawPreheatADC)
// {
//     // calculate the preheat voltage
//     double voltage = rawPreheatADC * (3.3 / mMaxResolution);
//     voltage *= 2.0;
//     return voltage * 1000.0;
//     // return (((rawPreheatADC / mMaxResolution) * mRefVoltage) * 2.0) * 1000.0;
// }