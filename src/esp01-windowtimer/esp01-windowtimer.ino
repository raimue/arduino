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

// Number of entries in a static array.
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// Globals
WiFiClientSecure httpClient;
AsyncTelegram2 telegram(httpClient);
Session telegramSession;
X509List telegramCertificate(telegram_cert);

// Reminder state
uint32_t gWindowOpenStart = 0;

// Pick a random entry from a message pool for more variety.
const char *pickOne(const char *list[], int count) {
    return list[random(count)];
}

String formatWindowMessage(const char *body) {
    unsigned long openMinutes = (millis() - gWindowOpenStart) / 60000UL;

    String full = body;

    if (openMinutes > 0) {
      String elapsed;
      if (openMinutes < 60) {
          elapsed = String(openMinutes) + " Minuten";
      } else {
          elapsed = String(openMinutes / 60) + " Std. " + String(openMinutes % 60) + " Min.";
      }

      full += " (";
      full += elapsed;
      full += ")";
    }

    return full;
}

// Message pools for more variety. One entry is picked at random per reminder.
const char *kStartMessages[] = {
    "Fenster ist offen! Ich erinnere dich in 5 Minuten ans Schließen.",
    "Frischluft! Fenster ist offen. Erste Erinnerung kommt in 5 Minuten.",
    "Okay, Fenster-Timer läuft! Ich melde mich in 5 Minuten wieder.",
    "Offenes Fenster erkannt. Lüften ist gut – ich passe auf die Zeit auf!",
    "Durchzug gestartet! Fenster ist offen, ich erinnere dich in 5 Minuten.",
    "Fenster auf – Timer an! Erinnerung kommt in 5 Minuten.",
    "Fenster-Timer aktiv! Ich melde mich in 5 Minuten.",
    "Frischluftmodus an! In 5 Minuten erinnere ich dich ans Schließen.",
};
const char *kGentleMessages[] = {
    "Zeit ist um! Mach das Fenster zu!",
    "5 Minuten sind rum – bitte Fenster schließen!",
    "Erinnerung: Das Fenster ist noch offen. Bitte zumachen!",
    "Es zieht schon rein! Zeit, das Fenster zu schließen.",
    "Frischluft reicht fürs Erste – Fenster bitte schließen!",
    "Kleiner Stups: Fenster bitte zumachen!",
    "Noch offen? Einmal Fenster schließen, bitte!",
    "Na, wie sieht's aus? Fenster bitte schließen!",
    "Tick, tack – Fensterzeit ist um! Bitte zumachen.",
    "Hallo? Das Fenster wartet aufs Schließen!",
    "Fünf Minuten frische Luft genügen – Fenster zu, bitte!",
    "Kurze Erinnerung: Fenster schließen nicht vergessen!",
    "Es wird kühl – mach bitte das Fenster zu!",
    "So, gelüftet ist gelüftet – Fenster bitte zu!",
};
const char *kUrgentMessages[] = {
    "Hey, nicht vergessen, du musst das Fenster zu machen!",
    "Das Fenster ist immer noch offen – jetzt wirklich zumachen!",
    "Schon eine ganze Weile offen! Bitte mach das Fenster zu.",
    "Die Heizung freut sich, wenn du das Fenster jetzt schließt.",
    "Los, Fenster zu – du schaffst das!",
    "Lüften ist vorbei – bitte Fenster schließen, sonst wird's kalt!",
    "Immer noch offen! Bitte jetzt das Fenster schließen.",
    "Du wolltest doch das Fenster zumachen – jetzt wäre gut!",
    "Achtung, Dauerlüftung! Fenster bitte schließen.",
    "Frierst du nicht schon? – Fenster zu, bitte!",
    "Erinnerung Nummer zwei: Fenster schließen!",
    "Komm schon, ein Griff – Fenster zu!",
};
const char *kFinalMessages[] = {
    "Letzte Warnung: MACH JETZT DAS FENSTER ZU!!!",
    "ERNSTHAFT: Fenster JETZT schließen!!!",
    "Das Fenster ist schon ewig offen – bitte SOFORT schließen!",
    "Finaler Alarm: Fenster zu, sonst heizt du zum Fenster raus!",
    "Okay, letzte Chance – FENSTER JETZT ZU, bitte!!!",
    "ES REICHT: Fenster SOFORT schließen!!!",
    "Dauerlüftung erkannt – FENSTER ZU, SOFORT!!!",
    "Letzter Aufruf: Fenster schließen, bitte!!!",
    "Stopp! Keine weitere Frischluft – FENSTER ZU!!!",
    "Alarmstufe Rot: FENSTER JETZT SCHLIESSEN!!!",
};

// Send a random reminder from the given pool, logging on failure.
void sendReminder(const char *list[], int count) {
    if (!telegram.sendTo(BOT_CHAT_ID, formatWindowMessage(pickOne(list, count)))) {
        Serial.println("Telegram send failed!");
    }
}

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

    randomSeed(micros() + ESP.getChipId());
    gWindowOpenStart = millis();

    TBMessage msg{};
    msg.chatId = BOT_CHAT_ID;
    msg.disable_notification = true;
    if (!telegram.sendMessage(msg, formatWindowMessage(pickOne(kStartMessages, ARRAY_SIZE(kStartMessages))))) {
        Serial.println("Telegram send failed!");
    }
}

// Loop
void loop() {
    static uint32_t state = 0;
    static uint32_t freq = 600;
    static uint32_t sleepTime = 5 * 60 * 1000;
    static uint32_t startTime = millis();

    uint32_t currentTime = millis();
    if (currentTime - startTime >= sleepTime) {
        state++;

        if (state <= 5) {
            sendReminder(kGentleMessages, ARRAY_SIZE(kGentleMessages));
            sleepTime = 60 * 1000;
            startTime = millis();
        } else if (state <= 10) {
            sendReminder(kUrgentMessages, ARRAY_SIZE(kUrgentMessages));
            freq = 300;
            sleepTime = 30 * 1000;
            startTime = millis();
        } else {
            sendReminder(kFinalMessages, ARRAY_SIZE(kFinalMessages));
            freq = 150;
            sleepTime = 30 * 1000;
            startTime = millis();
        }
    }

    if (state > 0) {
        static uint32_t lastBlink = 0;
        if (currentTime - lastBlink >= freq) {
            lastBlink = currentTime;
            digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
        }
    }

    ArduinoOTA.handle();
}
