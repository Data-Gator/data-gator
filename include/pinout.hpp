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
/** Built-in LED. BOOT STRAPPED!!! */
#define D13 12  
/** Unused, BOOT STRAPPED!!! */
#define D12 4 
/** Unused */
#define D11 16
/** Unused */
#define D10 17
/** Unused, BOOT STRAPPED!!! */
#define D9 2
/** Unused */
#define D7 13
/** Unused */
#define D6 14
/** Unused, BOOT STRAPPED!!! */
#define D5 0
/** Unused */
#define D3 26
/** Unused */
#define D2 25 

// Analog pins 
/** Unused, BOOT STRAPPED!!! */
/** Analog pin 4 */
#define A4 15
/** Analog pin 3 */
#define A3 35   
/** Analog pin 2 */
#define A2 34 
/** Analog pin 1 */
#define A1 39
/** Analog pin 0 */
#define A0 36

// Protocol and Bus Pins
// SPI
/** SPI SCK pin */
#define SCK 18
/** SPI MOSI pin */
#define MOSI 23
/** SPI MISO pin */
#define MISO 19
/** SPI CS pin */
#define CS D7

/** I2C SCL pin */
#define SCL 22
/** I2C SDA pin */
#define SDA 21

// UART
/** UART TX pin */
#define TX 1
/** UART RX pin */
#define RX 3

// Aggregator specific pins
/** uSD card power enable pin */
#define SD_PWR_EN 14
/** Power enable pin for sensors */
#define PWR_EN 26
//#define DQ 0
/** Done pin, for watchdog timer */
#define DONE 25
/** Mode select pin 1, (A2) */
#define SEL_1 34
/** Mode select pin 2, (A1) */
#define SEL_2 39 
/** Mode select pin 3, (A0) */
#define SEL_3 36
