#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include <Arduino.h>
#include <ArduinoJson.hpp>
#include <vector>
#include <SD.h>

// #include "config.hpp"
#include "AnalogSensor.hpp"
#include "Teros10.hpp"
#include "MIJ02LMS.hpp"
#include "SDCard.hpp"
// #include "../../include/SDCard.hpp"

#define TT_CLK 18
#define TT_MISO 19
#define TT_MOSI 23
#define TT_SS 13

typedef enum SensorType
{
    UNASSIGNED_TYPE,
    TEROS10_TYPE,
    MIJ02LMS_TYPE
} SensorType;

/**
 * @brief Class used to instantiate a factory to configure sensors based on configuration
 *        found in config.hpp
 *
 */
class SensorFactory
{
public:
    SensorFactory(String filename) : filename(filename)
    {
    }

    /**
     * @brief Return a reference to the vector of analog sensors
     *
     * @return const std::vector<std::shared_ptr<AnalogSensor>> &
     */
    const std::vector<std::shared_ptr<AnalogSensor>> &getAnalogSensors()
    {
        return this->analogSensors;
    };

    /**
     * @brief Initiate sensor ports and create relavent sensor objects for each port
     *
     */
    bool loadAnalogSensors()
    {
        // open connection to card
        this->initialize_sd_card();
        Serial.println("Reading Anolog Sensor config file...");

        // check if file exists, if not define default behavior
        if (!this->sdCard.exists(this->filename))
        {
            if (DEBUG)
            {
                Serial.println("File does not exist, using default sensor configuration.");
            }
            // default sensor configuration
            sensorPorts.push_back((SensorType)TEROS10_TYPE);
            sensorPorts.push_back((SensorType)TEROS10_TYPE);
            sensorPorts.push_back((SensorType)TEROS10_TYPE);
            // sensorPorts.push_back((SensorType)UNASSIGNED_TYPE);
        }
        else
        {
            // open the file
            File file = this->sdCard.open(filename, "r");
            Serial.println("Sensor configuration read");
            if (!file)
            {
                Serial.println("Failed to open sensor config JSON.");
                return false;
            }

            StaticJsonDocument<512> doc;
            // print the file contents
            Serial.println("Reading file...");

            DeserializationError error = deserializeJson(doc, file);
            file.close();
            this->close_card();

            // Print the entire JSON document
            Serial.println("Deserialized JSON:");
            serializeJson(doc, Serial);
            Serial.println(); // Add a newline for better readability

            if (error)
            {
                Serial.print("JSON parse failed: ");
                Serial.println(error.c_str());
                return false;
            }

            JsonObject analogPorts = doc["analog_ports"].as<JsonObject>();

            // iterate through the JSON object and assign sensor types
            for (JsonPair kv : analogPorts)
            {
                String port = kv.key().c_str();
                // int portNum = port.substring(5).toInt(); // get the number from the string
                String sensorType = kv.value().as<String>();

                Serial.printf("Port %s has sensor type %s\n", port.c_str(), sensorType.c_str());

                if (sensorType == "TEROS_10")
                {
                    sensorPorts.push_back((SensorType)TEROS10_TYPE);
                }
                else if (sensorType == "MIJ02LMS")
                {
                    sensorPorts.push_back((SensorType)MIJ02LMS_TYPE);
                }
                else
                {
                    sensorPorts.push_back((SensorType)UNASSIGNED_TYPE);
                }
            }
        }

        createAnalogSensors();

        return true;
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
    }

    /**
     * @brief Call before using SD card interface. Initializes connection to SD card.
     * -Borrowed from SDCard class.
     *
     * @returns `true` if successfully initialized.
     */
    bool initialize_sd_card()
    {
        const auto ok = this->sdCard.begin(TT_CLK, TT_MISO, TT_MOSI, TT_SS, &SPI);

        if (!ok)
        {
            Serial.println("[ERROR] failed to initialize sd card for JSON file read");
            return false;
        }

        Serial.println("[DEBUG] success initializing sd card logger for JSON file read");
        Serial.print("[DEBUG] bytes free =  ");
        double perc = ESP.getFreeHeap();
        Serial.println(perc);
        return true;
    }

    /**
     * Closes the connection to the SD card. This is effectively shutting down
     * the connection through the SPI interface.
     */
    void close_card()
    {
        this->sdCard.end();
    }

    /**
     * Opens a file in one of the two access modes, read only, or read/write.
     *
     * @param[in] mode `r`, `w`, etc
     *
     * @returns An open file object. If it fails, the error is printed and it should stop execution.
     */
    File open_file(const char *mode)
    {
        try
        {
            Serial.printf("\t-> trying to open file \'%s\' in mode -> %s\n", this->filename.c_str(), mode);
            return this->sdCard.open(this->filename.c_str(), mode);
        }
        catch (const std::exception &e)
        {
            Serial.println("print error in open");
            Serial.println(e.what());
        }
        catch (...)
        {
            Serial.println("ERROR opening file");
        }
    }

    std::vector<std::shared_ptr<AnalogSensor>> analogSensors; // will be list of sensors
    std::vector<SensorType> sensorPorts;                      // will be list of ports
    SDCard sdCard;
    String filename;
};

#endif // SENSOR_FACTORY_HPP