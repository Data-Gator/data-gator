#include <unity.h>
#include <Arduino.h>

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


void test_ds18b20(void){

    digitalWrite(PWR_EN, HIGH);

    // GPIO sensor is connected to
    // 0/D5
    const int one_wire_bus = 0;

    OneWire one_wire(one_wire_bus);

    DallasTemperature sensors(&one_wire);

    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);

    TEST_ASSERT_FLOAT_WITHIN(10, 20, temperatureC);

    digitalWrite(PWR_EN, LOW);
}



void test_all_analog_to_digital(void){
    int raw_values[4] = {0,0,0,0};
    
    // turn on sensors
    digitalWrite(PWR_EN, HIGH);
    delay(10000);
    // read from sensors
    raw_values[0] = ads.readADC_SingleEnded(0);
    raw_values[1] = ads.readADC_SingleEnded(1);
    raw_values[2] = ads.readADC_SingleEnded(2);
    raw_values[3] = ads.readADC_SingleEnded(3);

    bool sensors_non_zero = raw_values[0] > 0 && 
                            raw_values[1] > 0 && 
                            raw_values[2] > 0 && 
                            raw_values[3] > 0;

    // turn off sensors
    digitalWrite(PWR_EN, LOW);
    // read from sensors
    raw_values[0] = ads.readADC_SingleEnded(0);
    raw_values[1] = ads.readADC_SingleEnded(1);
    raw_values[2] = ads.readADC_SingleEnded(2);
    raw_values[3] = ads.readADC_SingleEnded(3);

    bool sensors_zero = raw_values[0] == 0 && 
                            raw_values[1] == 0 && 
                            raw_values[2] == 0 && 
                            raw_values[3] == 0;

    TEST_ASSERT_TRUE(sensors_non_zero && sensors_zero);
}

void test_analog_analog(void){
    int raw_value = 0;

    // turn on sensors
    digitalWrite(PWR_EN, HIGH);
    //delay(10000);

    raw_value = ads.readADC_SingleEnded(0);

    // turn off sensors
    //digitalWrite(PWR_EN, LOW);

    TEST_ASSERT_FLOAT_WITHIN(1, 1.75, raw_value * 0.0001875);
}


void test_analog_shallow(void){
    int raw_value = 0;

    // turn on sensors
    digitalWrite(PWR_EN, HIGH);
    //delay(10000);

    raw_value = ads.readADC_SingleEnded(1);

    // turn off sensors
    //digitalWrite(PWR_EN, LOW);

    TEST_ASSERT_FLOAT_WITHIN(1, 1.75, raw_value * 0.0001875);
}

void test_analog_middle(void){
    int raw_value = 0;

    // turn on sensors
    digitalWrite(PWR_EN, HIGH);
    //delay(10000);

    raw_value = ads.readADC_SingleEnded(2);

    // turn off sensors
    //digitalWrite(PWR_EN, LOW);

    TEST_ASSERT_FLOAT_WITHIN(1, 1.75, raw_value * 0.0001875);
}


void test_analog_deep(void){
    int raw_value = 0;

    // turn on sensors
    digitalWrite(PWR_EN, HIGH);
    //delay(10000);

    raw_value = ads.readADC_SingleEnded(3);

    // turn off sensors
    //digitalWrite(PWR_EN, LOW);

    TEST_ASSERT_FLOAT_WITHIN(1, 1.75, raw_value * 0.0001875);
}

void setup(void){

    pinMode(PWR_EN, OUTPUT);
    digitalWrite(PWR_EN, HIGH);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    pinMode(SD_PWR_EN, OUTPUT);
    digitalWrite(SD_PWR_EN, HIGH);

    // init ads
    ads.begin();

    Wire.begin();
    delay(2000);

    UNITY_BEGIN();
    
    // test DS18b20
    RUN_TEST(test_ds18b20);

    // test analog things
    RUN_TEST(test_analog_analog);
    RUN_TEST(test_analog_shallow);
    RUN_TEST(test_analog_middle);
    RUN_TEST(test_analog_deep);

    UNITY_END();

}

void loop(void){}
