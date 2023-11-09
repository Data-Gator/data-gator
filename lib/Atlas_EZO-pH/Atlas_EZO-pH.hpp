/**
 * @file Atlas_EZO-pH.hpp
 * @brief Implements I2C pH sensor which inherits from pHSensor.hpp
 *
 * Based on the Atlas EZO-pH example code for the sensor of the same name.
 *
 * @author Garrett Wells 
 * @date 03/09/2023
 */

#ifndef ATLAS_EZO_PH
#define ATLAS_EZO_PH


#include <Wire.h>
#include <pHSensor.hpp>
// default EZO-pH sensor I2C addresses
#define EZO_I2C_ADDR 99             //!< Default I2C address for sensor
#define EZO_I2C_SHALLOW_ADDR 0x01   //!< I2C address assigned to sensors buried at shallow depth (~1ft)
#define EZO_I2C_MIDDLE_ADDR 0x02    //!< I2C address assigned to sensors buried at medium depth  (~2ft)
#define EZO_I2C_DEEP_ADDR 0x03      //!< I2C address assigned to deepest sensors (~3ft)

/**
 * @brief Defines an I2C pH sensor which inherits from pHSensor.hpp. 
 *
 * This sensor uses the I2C bus and is designed to support the usage of 
 * multiple sensors with a single system. A three address standard is defined for 
 * connecting three sensors to the same microcontroller. If desired, however, 
 * more sensors may be connected as long as each is configured with its own address.
 *
 * See the configuration guides from the manufacturer or the guides written for the 
 * Data Gator project to configure the I2C address of your sensor.
 */
class AtlasEZOpH: public pHSensor{
    private:
        byte received_from_computer = 0; //!< we need to know how many characters have been received.
        byte serial_event = 0;           //!< a flag to signal when data has been received from the pc/mac/other.
        byte code = 0;                   //!< used to hold the I2C response code.
        char ph_data[32];                //!< we make a 32 byte character array to hold incoming data from the pH circuit.
        byte in_char = 0;                //!< used as a 1 byte buffer to store inbound bytes from the pH Circuit.
        byte i = 0;                      //!< counter used for ph_data array.
        int address = 99;                //!< default i2c address

    public:
        AtlasEZOpH(){};
        AtlasEZOpH(int address){
            this->address = address; 
        }

        /**
         * @brief Check if sensor present at address.
         *
         * Checks I2C bus for prescence of sensor at the specified address.
         * This is done by starting a transmission to the address, collecting the
         * error value. If the value is 0 then sensor is present.
         *
         * Note: This method will work for ANY sensor which shares this address. Thus 
         * caution should be taken to ensure that only the expected type of sensor is allowed
         * to have one of the addresses specified for shallow, middle, and deep sensors
         * in Atlas_EZO-pH.hpp.
         *
         * @returns A boolean flag, true if sensor connected at address.
         */
        bool sensor_at_address(int address){
            Wire.beginTransmission(address);
            byte error = Wire.endTransmission();

            if(error == 0){ // device connected at address
                return true;
            }else if(error != 2){
                return false;
            }

            return false;
        }


        double getpH(double){return 0.0;}


        /**
         * @brief Reads pH value from sensor as a double on the pH scale.
         *
         * @param[in] address The address of the sensor to read from (shallow, medium, or deep). Should be one of the defined addresses.
         *
         * @returns A double pH value on the range [0.001, 14.000]
         */
        double getpH(int address){
            char computerdata[20];           //we make a 20 byte character array to hold incoming data from a pc/mac/other.
            int reading_delay = 815;        // time required to read the sensor
            Serial.print("\treading from address "); 
            Serial.println(address);
            Wire.beginTransmission(address);
            computerdata[0] = 'r';
            computerdata[1] = 0;
            Wire.write(computerdata);                // signal sensor to take reading
            Wire.endTransmission();
            
            // delay for reading
            delay(reading_delay);

            Wire.requestFrom(address, 32, 1);   // request 32 bytes of data
            code = Wire.read();                 // read response code
                                                    
            switch (code) {							          //switch case based on what the response code is.
                case 1:                         		//decimal 1.
                  Serial.println("Success");    		//means the command was successful.
                  break;                        		//exits the switch case.

                case 2:                         		//decimal 2.
                  Serial.println("Failed");     		//means the command has failed.
                  break;                        		//exits the switch case.

                case 254:                       		//decimal 254.
                  Serial.println("Pending");    		//means the command has not yet been finished calculating.
                  break;                        		//exits the switch case.

                case 255:                       		//decimal 255.
                  Serial.println("No Data");    		//means there is no further data to send.
                  break;                        		//exits the switch case.
                                                        //
                default:
                  Serial.println("unknown code");
            }

            while(Wire.available()){
                in_char = Wire.read();
                ph_data[i] = in_char;
                i += 1;

                if(in_char == 0){
                    i = 0;
                    break;
                }
            }
            double pH_reading = std::atof(ph_data);

            return pH_reading;
        }

        /**
         * @brief Returns a pH measurement from the default sensor address. 
         *
         * This function is the "plug and play" option for reading from this 
         * sensor without configuration.
         *
         * @returns A double pH value on the range [0.001, 14.000].
         */
        double getpH(){
            return this->getpH(EZO_I2C_ADDR);
        }

        /**
         * @brief Get the pH reading as a string.
         *
         * @returns A character pointer to the data.
         */
        char* getpH_str(int address){
            //Serial.println(this->getpH(address));
            this->getpH(address);
            return ph_data;
            //return "-1.0";
        }

        void clear_pH_str(void){
            ph_data[0] = 0;
        }

        /**
         * @brief Get the string identifier for the Atlas EZO pH sensor. For use in MQTT topics and debugging.
         *
         * @returns A string "atlas_ezo_ph"
         */
        std::string getSensorType(void){
            return "atlas_ezo_ph";
        }

        /**
         * @brief Converts a pH voltage reading to a JSON object string for MQTT and debugging.
         *
         * @returns A JSON string with `PH_RAW` and `PH` fields which contain double values.
         */
        std::string toJSON(double voltage){
       		return "\"PH_RAW\": " + std::to_string(voltage) + ", \"PH\":" + std::to_string(this->getpH(voltage));
        }

};
#endif
