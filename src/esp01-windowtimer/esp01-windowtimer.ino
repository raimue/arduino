#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <AsyncTelegram2.h>
#include <TZ.h>
#include <time.h>

// Secrets
//#define WIFI_SSID ""
//#define WIFI_PASSWORD ""
//#define OTA_HOSTNAME ""
//#define OTA_PORT 8266
//#define OTA_PASSWORD ""
//#define BOT_TOKEN ""
//#define BOT_CHAT_ID 1234

// Board specifics
constexpr int LED_STATUS = 2;
#define MYTZ TZ_Europe_Berlin

// Globals
WiFiClientSecure httpClient;
AsyncTelegram2 telegram(httpClient);
Session telegramSession;
X509List telegramCertificate(telegram_cert);

// Setup
void setup() {
    pinMode(LED_STATUS, OUTPUT);
    digitalWrite(LED_STATUS, LOW);

    Serial.begin(74880);
    Serial.println("Booting");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
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
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());

    // Update current time via NTP
    configTime(MYTZ, WiFi.gatewayIP().toString());

    httpClient.setSession(&telegramSession);
    httpClient.setTrustAnchors(&telegramCertificate);
    httpClient.setBufferSizes(1024, 1024);

    telegram.setUpdateTime(2000);
    telegram.setTelegramToken(BOT_TOKEN);

    telegram.begin();
    Serial.printf("Telegram bot @%s started\r\n", telegram.getBotName());
}

// Loop
void loop() {
    static uint32_t state = 0;
    static uint32_t freq = 10000;
    static uint32_t sleepTime = 1 * 60 * 1000;
    static uint32_t startTime = millis();

    uint32_t currentTime = millis();
    if (currentTime - startTime >= sleepTime) {
        state++;

        if (state <= 5) {
            telegram.sendTo(BOT_CHAT_ID, "Zeit ist um! Mach das Fenster zu!");
            sleepTime = 60 * 1000;
            startTime = millis();
        } else if (state <= 10) {
            telegram.sendTo(BOT_CHAT_ID, "Hey, nicht vergessen, du musst das Fenster zu machen!");
            freq = 5000;
            sleepTime = 30 * 1000;
            startTime = millis();
        } else {
            telegram.sendTo(BOT_CHAT_ID, "Letzte Warnung: MACH JETZT DAS FENSTER ZU!!!");
            freq = 1000;
            sleepTime = 30 * 1000;
            startTime = millis();
        }
    }

    if (state > 0) {
        static uint32_t count = 0;
        count++;
        if (count % freq == 0) {
            count = 0;
            digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
        }
    }

    ArduinoOTA.handle();
}
