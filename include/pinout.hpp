/**
 * @file pinout.hpp
 * @brief Maps pin numbers to defined variables.
 * @defgroup   PINOUT pinout
 *
 * @brief      This file implements/defines pinout for the data gator board starting with revision 3.
 *
 * @author     Garrett Wells
 * @date       2022
 */

// Raw pin definitions for firebeetle
// Digital Pins
#define D13 12  //!< Built-in LED. BOOT STRAPPED!!! 
#define D12 4   //!< Unused, BOOT STRAPPED!!!
#define D11 16  //!< Unused
#define D10 17  //!< Unused
#define D9 2    //!< Unused, BOOT STRAPPED!!!
#define D7 13   //!< Unused
#define D6 14   //!< Unused
#define D5 0    //!< Unused, BOOT STRAPPED!!!
#define D3 26   //!< Unused
#define D2 25   //!< Unused

// Analog pins 
#define A4 15   //!< Analog pin 4, Unused bc BOOT STRAPPED!!!
#define A3 35   //!< Analog pin 3 
#define A2 34   //!< Analog pin 2 
#define A1 39   //!< Analog pin 1
#define A0 36   //!< Analog pin 0 

// Protocol and Bus Pins
// SPI
#define SCK 18  //!< SPI SCK pin 
#define MOSI 23 //!< SPI MOSI pin
#define MISO 19 //!< SPI MISO pin 
#define CS D7   //!< SPI CS pin

#define SCL 22  //!< I2C SCL pin
#define SDA 21  //!< I2C SDA pin

// UART
#define TX 1    //!< UART TX pin
#define RX 3    //!< UART RX pin 

// Aggregator specific pins
#define SD_PWR_EN 14    //!< uSD card power enable pin
#define PWR_EN 26       //!< Power enable pin for sensors
//#define DQ 0
#define DONE 25         //!< Done pin, for watchdog timer
#define SEL_1 34        //!< Mode select pin 1, (A2)
#define SEL_2 39        //!< Mode select pin 2, (A1) 
#define SEL_3 36        //!< Mode select pin 3, (A0)
