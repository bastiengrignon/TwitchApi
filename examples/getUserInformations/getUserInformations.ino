#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <TwitchApi.hpp>

#define WLAN_SSID   "*************"
#define WLAN_PASS   "*************"

/******************
 * Instantiations *
 ******************/
WiFiClientSecure client;
HTTPClient http;
TwitchApi twitch(client, "twitch_login", "twitch_client_id", "twitch_client_secret", "twitch_access_token");

/*************
 * Variables *
 *************/

unsigned long lastUpdateSensor{0};
unsigned long sensorUpdateInterval{2000};

void setup() {
    Serial.begin(115200);
    initHTTPClient();
    initWiFiConnection();
}

void loop() {
    if ((millis() - lastUpdateSensor) > sensorUpdateInterval) {
        twitch.getUserInformation("twitch_login");
        lastUpdateSensor = millis();
    }
}

/***********************
     Initialisation
 ***********************/
const void initHTTPClient() {
    const uint16_t httpsPort{443};
    client.setInsecure();
    client.connect("https://api.twitch.tv", httpsPort);
}

const void initWiFiConnection() {
    WiFi.begin(WLAN_SSID, WLAN_PASS);

    Serial.print("Wifi connection.");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}