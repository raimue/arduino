#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <DHT.h>

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
const int DHT_POWER = 3;
DHT dht(DHT_DATA, DHT22);

// Status LED
const int LED_STATUS = 1;

// Enable ADC for reading VCC
ADC_MODE(ADC_VCC);

// default loop delay
unsigned long sleeptime = 10000;

Ticker sleepTicker;
unsigned long startTime;

#define MAX_LOOP_TIME_MS 10000

void sleepFunc() {
    const int elapsed = millis() - startTime;
    // If this sleep happened because of timeout, clear the
    // Wifi settings. (Maybe the AP channel changed, etc.)
    if (elapsed >= MAX_LOOP_TIME_MS) {
        WiFi.disconnect();
    }
    // sleep n microseconds
    ESP.deepSleep(sleeptime * 1000, WAKE_RF_DEFAULT);
    // It can take a while for the ESP to actually go to sleep.
    // When it wakes up we start again in setup().
    delay(5000);
}

// only runs once on boot
void setup() {
    // watchdog for one minute maximum runtime
    startTime = millis();
    sleepTicker.once_ms(60000, &sleepFunc);

#ifdef DEBUG
    // Initializing serial port for debugging purposes
    Serial.begin(74880);
    delay(10);
#endif

    // Use RX/TX pins as normal GPIOs
#ifndef DEBUG
    pinMode(LED_STATUS, OUTPUT);
    pinMode(DHT_POWER, OUTPUT);
    digitalWrite(DHT_POWER, LOW);
#endif

    // Blink status LED
#ifndef DEBUG
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
    Serial.println(WIFI_SSID);
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

void loop() {
    // Temporary variables
    char celsiusTemp[7];
    char humidityTemp[7];
    float t;
    float h;
    float vcc;
    unsigned long timeout;

#ifndef DEBUG
    // Enable DHT sensor
    digitalWrite(DHT_POWER, HIGH);
#endif
    dht.begin();
    // give sensor time to stabilize, datasheet recommends >2s
    delay(2000);

    // Update status LED for activity
#ifndef DEBUG
    digitalWrite(LED_STATUS, LOW);
    delay(600);
    digitalWrite(LED_STATUS, HIGH);
#endif

    // Connect to server to submit measurement
    WiFiClient client;
    if (!client.connect(SERVER_IP, SERVER_PORT)) {
#ifdef DEBUG
        Serial.println("Client connection failed!");
#endif
        goto error;
    }

    // Print MAC
    client.print(WiFi.macAddress());
    client.print("  ");

    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    t = dht.readTemperature();
    h = dht.readHumidity();

    // Check if any reads failed and exit
    if (isnan(h) || isnan(t)) {
        client.print("FAILED ");
    } else {
        // Convert float to string
        dtostrf(t, 6, 2, celsiusTemp);
        dtostrf(h, 6, 2, humidityTemp);

        // Print values
        client.print(celsiusTemp);
        client.print(" C  ");
        client.print(humidityTemp);
        client.print(" %  ");
    }
    // Print input voltage
    vcc = ((float)ESP.getVcc())/1024;
    client.print(vcc);
    client.println(" V");

    // yield CPU
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
        long stime = client.parseInt();
#ifdef DEBUG
        Serial.print("Server reply: ");
        Serial.println(stime);
#endif
        if (stime > 0) {
            sleeptime = stime;
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
    // increase backoff
    if (sleeptime < 5*60*1000) {
        sleeptime *= 2;
    }

out:
    // disconnect
    client.stop();

#ifndef DEBUG
    // disable DHT sensor
    digitalWrite(DHT_POWER, LOW);
#endif

    // go to sleep
    sleepFunc();
}
