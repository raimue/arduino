#include <Arduino.h>
#include <LiquidCrystal.h>

#define LED 13

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    // Disable LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // Set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("Hello, World!");

    Serial.println("Initialization done!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}
