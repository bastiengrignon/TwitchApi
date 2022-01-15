// Created by basgr on 15/01/2022.

#include "TwitchApi.hpp"

TwitchApi::TwitchApi(WiFiClientSecure &client, const String clientLogin, const String clientId, const String clientSecret, const String accessToken) {
    this->client = &client;
    this->_clientLogin = clientLogin;
    this->_clientId = clientId;
    this->_clientSecret = clientSecret;
    this->_accessToken = accessToken;
}

const void TwitchApi::setDebug(const bool debug) {
    this->_debug = debug;
}

const String TwitchApi::updateAccessToken() {
    const String fetchedData = makePOSTRequest(this->twitchApiRegistration +"/oauth2/token",
                                            "client_id=" + this->_clientId +
                                            "&client_secret=" + this->_clientSecret +
                                            "&grant_type=client_credentials"
                                            "&scope=channel:read:subscriptions");
    DynamicJsonDocument doc(512);
    deserializeJson(doc, fetchedData);
    this->_accessToken = doc["access_token"].as<String>();
    if (this->_debug) {
        Serial.println("--- Twitch Access Token ---");
//        serializeJsonPretty(doc, Serial);
    }
}

TwitchUser TwitchApi::getUserInformation(String twitchLogin) {
    const String response = makeGETRequest(this->twitchApi + "/helix/users?login=" + twitchLogin);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    if (this->_debug) {
        Serial.println("--- getUserInformation ---");
        serializeJsonPretty(doc, Serial);
        Serial.println();
    }
    TwitchUser user;

    JsonObject data = doc["data"][0];
    user.id = data["id"].as<String>();
    user.login = data["login"].as<String>();
    user.displayName = data["display_name"].as<String>();
    user.type = data["type"].as<String>();
    user.broadCasterType = data["broadcaster_type"].as<String>();
    user.description = data["description"].as<String>();
    user.profileImageUrl = data["profile_image_url"].as<String>();
    user.offlineImageUrl = data["offline_image_url"].as<String>();
    user.viewCount = data["view_count"].as<long>();

    return user;
}

const String TwitchApi::httpRequest(const String httpMethod, const String endpoint, const String data) {
    HTTPClient http;
    String payload{""};


    this->client->setInsecure();
    this->client->connect(twitchApi, httpsPort);
    Serial.println("--- Everything went well ---");

    if (this->_debug) {
        DynamicJsonDocument doc(512);
        doc["Client-Id"].set(this->_clientId);
        doc["Authorization"].set("Bearer " + this->_accessToken);
        serializeJsonPretty(doc, Serial);
    }

    if (http.begin(endpoint)) {
        int httpResponseCode{-1};
        if (httpMethod == "POST") {
            httpResponseCode = http.POST(data);
        } else if (httpMethod == "GET") {
            http.addHeader("Host", "api.twitch.tv");
            http.addHeader("Client-Id", this->_clientId);
            http.addHeader("Authorization", "Bearer " + this->_accessToken);
            http.addHeader("Cache-Control", "no-cache");
            httpResponseCode = http.GET();
        }

        if (httpResponseCode > 0 && (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY)) {
            payload = http.getString();
        } else {
            if (this->_debug) {
                DynamicJsonDocument doc(512);
                doc["method"].set(httpMethod);
                doc["HTTP code"].set(httpResponseCode);
                doc["HTTP error"].set(http.errorToString(httpResponseCode));
                doc["data"].set(data);
                serializeJsonPretty(doc, Serial);
            }
        }
        http.end();
    } else {
        if (this->_debug) {
            Serial.println("Something went wrong ! Url: " + endpoint);
            DynamicJsonDocument doc(512);
            deserializeJson(doc, http.getString());
            serializeJsonPretty(doc, Serial);
        }
    }
    return payload;
}

const String TwitchApi::makeGETRequest(const String endpoint) {
    return this->httpRequest("GET", endpoint);
}

const String TwitchApi::makePOSTRequest(const String endpoint, const String data) {
    return this->httpRequest("POST", endpoint, data);
}
