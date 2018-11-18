#include <Arduino.h>

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Replace with your network details
//#define WIFI_SSID    ""
//#define WIFI_PASS    ""
//#define SERVER_IP    "192.168.0.1"
//#define SERVER_PORT  1234
#include "wifi-auth.h"

#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s

// Enable debugging over serial
//#define DEBUG

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
        delay(50);
        digitalWrite(LED_STATUS, HIGH);
        delay(50);
    }
#endif

    // Connecting to WiFi network
#ifdef DEBUG
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif

    // Update WiFi data if SSID changed
    if (WiFi.SSID() != WIFI_SSID) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        WiFi.persistent(true);
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
    }

    // TODO: go to sleep if no connection
    while (WiFi.status() != WL_CONNECTED) {
#ifdef DEBUG
        Serial.print(".");
#endif
        digitalWrite(LED_STATUS, LOW);
        delay(250);
        digitalWrite(LED_STATUS, HIGH);
        delay(250);
    }
#ifdef DEBUG
    Serial.println("");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
#endif

    // Enable DHT sensor
    dht.begin();

    // Update status LED for connection
#ifndef DEBUG
    for (int i = 0; i < 2; i++) {
        digitalWrite(LED_STATUS, LOW);
        delay(100);
        digitalWrite(LED_STATUS, HIGH);
        delay(100);
    }
#endif
}

// runs over and over again
void loop() {
    // Temporary variables
    char celsiusTemp[7];
    char humidityTemp[7];
    float t;
    float h;
    int sleeptime = 30000;
    unsigned long timeout;

    // Connect to server to submit measurement
    WiFiClient client;
    if (!client.connect(SERVER_IP, SERVER_PORT)) {
#ifdef DEBUG
        Serial.println("Client connection failed!");
#endif
        // Update status LED for failure
        for (int i = 0; i < 2; i++) {
            digitalWrite(LED_STATUS, LOW);
            delay(100);
            digitalWrite(LED_STATUS, HIGH);
            delay(100);
        }
        goto error;
    }

    // Update status LED for activity
#ifndef DEBUG
    digitalWrite(LED_STATUS, LOW);
    delay(1000);
    digitalWrite(LED_STATUS, HIGH);
#endif

    // Print MAC
    client.print(WiFi.macAddress());
    client.print("  ");

    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    t = dht.readTemperature();
    h = dht.readHumidity();

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

    // Wait for answer from server
    timeout = millis();
    while (!client.available()) {
        if (millis() - timeout > 5000) {
#ifdef DEBUG
            Serial.println("Server reply timeout");
#endif
            goto error;
        }
    }

    // Read answer from server
    while (client.available()) {
        int tmp = client.parseInt();
#ifdef DEBUG
        Serial.print("Server reply: ");
        Serial.println(tmp);
#endif
        if (tmp > 0) {
            sleeptime = tmp;
        }
    }

    // success
    goto out;

error:
    // Update status LED for failure
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_STATUS, LOW);
        delay(100);
        digitalWrite(LED_STATUS, HIGH);
        delay(100);
    }

out:
    // disconnect
    client.stop();
    // go to sleep
    delay(sleeptime);
}
