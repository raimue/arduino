#include <Arduino.h>

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network details
//const char* ssid = "";
//const char* password = "";
#include "wifi-auth.h"

#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s

#define SERVER_PORT 1337

WiFiServer server(SERVER_PORT);

// DHT Sensor
const int DHTPin = 2;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];

// only runs once on boot
void setup() {
    // Initializing serial port for debugging purposes
    Serial.begin(115200);
    delay(10);

    dht.begin();

    // Connecting to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // do not write WiFi data to flash
    WiFi.persistent(false);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    // Starting the web server
    server.begin();
    Serial.println("Server running on port " STRINGIFY(SERVER_PORT));
}

// runs over and over again
void loop() {
    // Listenning for new clients
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New client");
        if (client.connected()) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t)) {
                Serial.println("Failed to read from DHT sensor!");
                strcpy(celsiusTemp,"Failed");
                strcpy(humidityTemp, "Failed");
            }
            else{
                // Computes temperature values in Celsius and Humidity
                float hic = dht.computeHeatIndex(t, h, false);
                dtostrf(hic, 6, 2, celsiusTemp);
                dtostrf(h, 6, 2, humidityTemp);

                // You can delete the following Serial.print's, it's just for debugging purposes
                Serial.print("Humidity: ");
                Serial.print(humidityTemp);
                Serial.print(" %\t Temperature: ");
                Serial.print(celsiusTemp);
                Serial.print(" *C\t Heat index: ");
                Serial.print(hic);
                Serial.print(" *C");
            }
            client.println(WiFi.macAddress());
            client.print(celsiusTemp);
            client.println(" C");
            client.print(humidityTemp);
            client.println(" %");
            delay(1);
        }
        // closing the client connection
        delay(1);
        client.stop();
        Serial.println("Client disconnected.");
    }
}
