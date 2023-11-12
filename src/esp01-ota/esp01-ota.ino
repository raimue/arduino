#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

constexpr int LED_STATUS = 2;

// Replace with your network details
//#define WIFI_SSID ""
//#define WIFI_PASSWORD ""
//#define OTA_HOSTNAME ""
//#define OTA_PORT 8266
//#define OTA_PASSWORD ""

void setup() {
    Serial.begin(74880);
    Serial.println("Booting");

    pinMode(LED_STATUS, OUTPUT);
    digitalWrite(LED_STATUS, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("WiFi connected!");

#ifdef OTA_HOSTNAME
    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(OTA_HOSTNAME);
#endif
#ifdef OTA_PORT
    // Port defaults to 8266
    ArduinoOTA.setPort(OTA_PORT);
#endif
#ifdef OTA_PASSWORD
    // No authentication by default
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    ArduinoOTA.handle();

    static uint32_t count = 0;
    count++;
    if (count % 10000 == 0) {
        count = 0;
        digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
    }
}
