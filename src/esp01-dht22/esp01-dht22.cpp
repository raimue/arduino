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

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];

// only runs once on boot
void setup() {
#ifdef DEBUG
    // Initializing serial port for debugging purposes
    Serial.begin(115200);
    delay(10);
#endif

    // Enable DHT sensor
    dht.begin();

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

    // Starting the web server
    server.begin();

#ifdef DEBUG
    Serial.println("Server running on port " STRINGIFY(SERVER_PORT));
#endif
}

// runs over and over again
void loop() {
    // Listenning for new clients
    WiFiClient client = server.available();

    if (client) {
#ifdef DEBUG
        Serial.println("New client");
#endif
        if (client.connected()) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t)) {
#ifdef DEBUG
                Serial.println("Failed to read from DHT sensor!");
#endif
                strcpy(celsiusTemp,"Failed");
                strcpy(humidityTemp, "Failed");
            }
            else{
                // Computes temperature values in Celsius and Humidity
                float hic = dht.computeHeatIndex(t, h, false);
                dtostrf(hic, 6, 2, celsiusTemp);
                dtostrf(h, 6, 2, humidityTemp);

#ifdef DEBUG
                // You can delete the following Serial.print's, it's just for debugging purposes
                Serial.print("Humidity: ");
                Serial.print(humidityTemp);
                Serial.print(" %\t Temperature: ");
                Serial.print(celsiusTemp);
                Serial.print(" *C\t Heat index: ");
                Serial.print(hic);
                Serial.println(" *C");
#endif
            }
            client.print(WiFi.macAddress());
            client.print("  ");
            client.print(celsiusTemp);
            client.print(" C  ");
            client.print(humidityTemp);
            client.println(" %");
            delay(1);
        }
        // closing the client connection
        delay(1);
        client.stop();

#ifdef DEBUG
        Serial.println("Client disconnected.");
#endif
    }
}
