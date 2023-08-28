#include <unity.h>
#include <string.h>
#include <Arduino.h>

#include <config.hpp>

#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <OneWire.h>

#include <DallasTemperature.h>
#include <OWMAdafruit_ADS1015.h>
#include <Adafruit_MAX1704X.h>
#include <Adafruit_BusIO_Register.h>


#define D9 2    // built-in LED
#define LED_BUILTIN D9
#define D7 13   // CS
#define CS D7
#define D5 0    // DQ
#define DQ D5
#define D6 14   // SD_PWR_EN
#define SD_PWR_EN D6
#define D3 26   // PWR_EN
#define PWR_EN D3


#define A2 34 
#define SEL_1 A2
#define A1 39
#define SEL_2 A1
#define A0 36
#define SEL_3 A0


Adafruit_MAX17048 maxlipo;
Adafruit_ADS1115 ads;    

// show that the power switches are working if the LEDs are 
// attached
void test_basic(void){
    pinMode(PWR_EN, OUTPUT);
    pinMode(SD_PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, HIGH);
    digitalWrite(SD_PWR_EN, HIGH);

    TEST_ASSERT_TRUE(true);
}

void test_wifi(void){
    WiFi.begin(NETWORK, PSSWD);

    int i = 0;

    while(i < 20){
        uint8_t status = WiFi.status();
        if(status == 3) TEST_ASSERT_TRUE(status == 3);
        switch(WiFi.status()){
            case WL_NO_SSID_AVAIL:
                  TEST_MESSAGE("WL_NO_SSID_AVAIL");
                  TEST_FAIL();
                  break;

            case WL_CONNECT_FAILED:
                  TEST_MESSAGE("WL_CONNECT_FAILED");
                  TEST_FAIL();
                  break;

            case WL_CONNECTION_LOST:
                  TEST_MESSAGE("WL_CONNECTION_LOST");
                  TEST_FAIL();
                  break;

            case WL_CONNECTED:
                  TEST_ASSERT_EQUAL(WiFi.status(), WL_CONNECTED);
                  i = 20;
                  break;

            default:
                  TEST_MESSAGE("status is");
                  TEST_MESSAGE(std::to_string(status).c_str());
                  break;

        }
        i++;
        delay(1000);
    }
}

void test_mode_select(void){
    pinMode(SEL_1, INPUT);
    pinMode(SEL_2, INPUT);
    pinMode(SEL_3, INPUT);

    bool switches_non_zero = analogRead(SEL_1) > 0 && analogRead(SEL_2) < 500 && analogRead(SEL_3) > 0;

    TEST_ASSERT_TRUE(switches_non_zero);
}



void test_init_fuel_gauge(void){
    TEST_ASSERT_TRUE(maxlipo.begin());
}


void test_fuel_gauge_read(void){
    maxlipo.wake();
    maxlipo.quickStart();

    delay(2000);

    TEST_ASSERT_FLOAT_WITHIN(2, 4, maxlipo.cellVoltage());
}


void test_sd_card_init(void){
    TEST_ASSERT_TRUE(SD.begin(CS, SPI, 8000000, "/sd", 5, true));
}


void setup(void){
    //Serial.begin(9600);
    // wait 2secs for serial interface to come up
    //delay(2000);

    Wire.begin();
    delay(2000);
    
    SD.begin(CS, SPI, 8000000, "/sd", 5, true);

    pinMode(PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, LOW);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    pinMode(SD_PWR_EN, OUTPUT);
    digitalWrite(SD_PWR_EN, LOW);

    ads.begin();

    UNITY_BEGIN();

    RUN_TEST(test_basic); 
    RUN_TEST(test_wifi);
    RUN_TEST(test_mode_select);
    
    // test fuel gauge
    RUN_TEST(test_init_fuel_gauge);
    RUN_TEST(test_fuel_gauge_read);

    // test sd card
    RUN_TEST(test_sd_card_init);

    UNITY_END();
}

void loop(void){}
