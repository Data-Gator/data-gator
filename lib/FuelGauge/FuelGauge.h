#include "Adafruit_MAX1704X.h"
#include <Adafruit_BusIO_Register.h>

Adafruit_MAX17048 maxlipo;

void setup(){
    if(!maxlipo.begin())
    {
        if(DEBUG)Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    }

  Serial.print(F("Found MAX17048"));
  Serial.print(F(" with Chip ID: 0x")); 
  Serial.println(maxlipo.getChipID(), HEX);
}

void loop() {
  Serial.print(F("Batt Voltage: ")); Serial.print(maxlipo.cellVoltage(), 3); Serial.println(" V");
  Serial.print(F("Batt Percent: ")); Serial.print(maxlipo.cellPercent(), 1); Serial.println(" %");
  Serial.println();

  delay(2000);  // dont query too often!
}
