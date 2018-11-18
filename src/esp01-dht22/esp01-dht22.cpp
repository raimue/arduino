#include <Arduino.h>

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Replace with your network details
//const char* ssid = "";
//const char* password = "";
#include "wifi-auth.h"

#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s

#define SERVER_PORT 1337

// Enable debugging over serial
//#define DEBUG

WiFiServer server(SERVER_PORT);

// DHT22 Sensor
const int DHT_DATA = 2;
DHT dht(DHT_DATA, DHT22);

// Status LED
const int LED_STATUS = 1;

// only runs once on boot
void setup() {
#ifdef DEBUG
    // Initializing serial port for debugging purposes
    Serial.begin(115200);
    delay(10);
#endif

    // Use TX pin for status LED only
#ifndef DEBUG
    pinMode(LED_STATUS, OUTPUT);
#endif

    // Blink status LED
#ifndef DEBUG
    pinMode(LED_STATUS, OUTPUT);
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_STATUS, LOW);
        delay(100);
        digitalWrite(LED_STATUS, HIGH);
        delay(100);
    }
#endif

    // Connecting to WiFi network
#ifdef DEBUG
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif

    // do not write WiFi data to flash
    WiFi.persistent(false);

    WiFi.begin(ssid, password);

    // TODO: go to sleep if no connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
#ifdef DEBUG
    Serial.println("");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
#endif

    // Enable DHT sensor
    dht.begin();

    // Starting the web server
    server.begin();

    // Update status LED for connection
#ifndef DEBUG
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_STATUS, LOW);
        delay(500);
        digitalWrite(LED_STATUS, HIGH);
        delay(500);
    }
#endif

#ifdef DEBUG
    Serial.println("Server running on port " STRINGIFY(SERVER_PORT));
#endif
}

// runs over and over again
void loop() {
    // Listenning for new clients
    WiFiClient client = server.available();

    if (client) {
        if (client.connected()) {
            // Temporary variables
            char celsiusTemp[7];
            char humidityTemp[7];

            // Print MAC
            client.print(WiFi.macAddress());
            client.print("  ");

            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Check if any reads failed and exit
            if (isnan(h) || isnan(t)) {
                client.println("FAILED");
            } else {
                // Convert float to string
                dtostrf(t, 6, 2, celsiusTemp);
                dtostrf(h, 6, 2, humidityTemp);

                // Print values
                client.print(celsiusTemp);
                client.print(" C  ");
                client.print(humidityTemp);
                client.println(" %");
            }
            // yield CPU to schedule socket communication
            delay(1);
        }
        // closing the client connection
        delay(1);
        client.stop();
    }
}
