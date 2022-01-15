// Created by basgr on 15/01/2022.

#ifndef TwitchApi_HPP
#define TwitchApi_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

struct TwitchUser {
    String id;
    String login;
    String displayName;
    String type;
    String broadCasterType;
    String description;
    String profileImageUrl;
    String offlineImageUrl;
    long viewCount;
};

class TwitchApi {
public:
    TwitchApi(WiFiClientSecure &client, const String clientLogin, const String clientId, const String clientSecret, const String accessToken = "");
    const void setDebug(const bool debug);
    const String updateAccessToken();
    TwitchUser getUserInformation(String twitchLogin);

private:
    const String httpRequest(const String httpMethod, const String endpoint, const String data = "");
    const String makePOSTRequest(const String endpoint, const String data);
    const String makeGETRequest(const String endpoint);

    const unsigned int httpsPort{443};
    const String twitchApi{"https://api.twitch.tv"};
    const String twitchApiRegistration{"https://id.twitch.tv"};

    bool _debug{true}; // TODO: change to false when in prod
    WiFiClientSecure *client;
    String _clientId{""};
    String _clientSecret{""};
    String _accessToken{""};
    String _clientLogin{""};
    uint8_t _clientLoginId{0};
};
#endif
