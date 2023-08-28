/**
 * @file config.hpp
 * @brief Defines configuration options for network, sensor readings, etc 
 *
 * Project Configuration Header
 *     generated from config.ini using config_generator.py
 */
/** WiFi network SSID */
#define NETWORK "network_ssid_here"
/** WiFi network password */
#define PSSWD "network_password_here"
/** MQTT broker network address */
#define MQTT_BROKER_ADDR "192.168.50.10"
/** MQTT broker port */
#define MQTT_PORT 1883
/** Global serial debug output flag */
#define DEBUG 1
/** WiFi timeout in seconds */
#define WIFI_TIMEOUT 10000
/** Number of timer ticks/power resets */
#define MAX_COUNT 120
/** Frequency with which the device checks for new firmware version on server */
#define OTA_FREQ 60
/** Ticks/minutes between volumetric water content sensor readings */
#define VWC_FREQ 5
/** Ticks/minutes between temperature and humidity sensor readings */
#define HT_FREQ 5
/** Ticks/minutes between device telemetry reports */
#define TLM_FREQ 5
