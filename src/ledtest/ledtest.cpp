#include <Arduino.h>

#define FIRST 8
#define LAST 13

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");
    for (uint8_t i = FIRST; i <= LAST; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    Serial.println("Initialization done!");
}

void loop() {
    for (uint8_t i = FIRST; i <= LAST; i++) {
        digitalWrite(i, HIGH);
        delay(500);
    }
    for (uint8_t i = FIRST; i <= LAST; i++) {
        digitalWrite(i, LOW);
        delay(500);
    }
}
