#include <Arduino.h>

unsigned long sleeptime = 3000;

void sleepFunc() {
    // sleep n microseconds
    ESP.deepSleep(sleeptime * 1000, WAKE_RF_DEFAULT);
    // It can take a while for the ESP to actually go to sleep.
    // When it wakes up we start again in setup().
    delay(5000);
}

void setup() {
    Serial.begin(74880);
    Serial.print("Alive! ");
    Serial.println(millis());

    while (!Serial.available()) {
        Serial.print(".");
        delay(500);
    }
}

void loop() {
    Serial.println("");
    Serial.print("Going to sleep!");
    sleepFunc();
}
