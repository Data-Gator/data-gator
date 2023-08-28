/**
 * @defgroup   FIREBEETLE_SLEEP firebeetle sleep
 *
 * @brief      This file implements firebeetle sleep (ESP32)
 *
 * @author     Garrett Wells
 * @date       2022
 * @file firebeetle_sleep.hpp
 */
#include <Arduino.h>
#include <esp_pm.h>


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;


/**
 * @brief      Configure the ESP32 for sleep and then enter deep sleep.
 *
 * @param[in]  time_to_sleep  time to elapse in seconds before waking up
 */
void deep_sleep(uint16_t time_to_sleep){
  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
  /*
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  //esp_bt_controller_disable();

  esp_deep_sleep_start();
}

/**
 * @brief Hibernate for \p time_to_sleep seconds
 *
 * @param[in] time_to_sleep The number of seconds to put the device to sleep for
 */
void hibernate(uint16_t time_to_sleep) {
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);
    
    deep_sleep(time_to_sleep);
}

/**
 * @brief Puts the device into light sleep
 *
 * Light sleep allows some types of power sections to continue 
 * operating.
 */
void light_sleep(){
  //esp_pm_configure();
  esp_light_sleep_start(); 
}

/**
 * @brief Convert seconds to micro-seconds
 *
 * @param[in] seconds
 */
uint16_t seconds_to_uS(uint16_t seconds){
  return seconds * uS_TO_S_FACTOR;
}

/**
 * @brief Method to print the reason by which ESP32 
 * has been awaken from sleep
 */
void print_wakeup_reason(){
  if(DEBUG){
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch(wakeup_reason)
    {
      case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
      case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
      case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
      case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
      case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
      default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
    }
  }
}
