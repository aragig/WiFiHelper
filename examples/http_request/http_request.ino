#include <Arduino.h>
#include "WiFiHelper.h"

#ifndef WIFI_SSID
#define WIFI_SSID ""  // TODO WiFi SSID (2.4GHz only)
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""  // TODO WiFiパスワード
#endif

#include <HTTPClient.h>

WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD, "WiFiHelper");

struct HttpResponse {
    int code;
    String body;
};

HttpResponse http_request(String url) {
    HttpResponse response;
    response.code = -1;
    response.body = "";

    if (!wifi.begin()) {
        response.code = -99;
        response.body = "Failed to initialize WiFi";
        return response; // WiFi初期化失敗
    }

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, url)) {
        response.code = -98;
        response.body = "Failed HTTPClient begin!";
        return response; // HTTPクライアント初期化失敗
    }

    http.addHeader("Content-Type", "application/json"); // コンテントタイプ指定

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        // 正常な応答があった場合、サーバーからの応答を取得（必要に応じて）
        response.code = httpResponseCode;
        response.body = http.getString();
    } else {
    }

    http.end(); // HTTP接続終了
    return response;
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    wifi.begin();

}

void loop() {
    String url = "http://apppppp.com/jojo.json";

    Serial.println("HTTP Request... ");

    HttpResponse res = http_request(url);
    Serial.print("HTTP Response code: "); Serial.println(res.code);
    Serial.print("HTTP Response body: ");
    Serial.println(res.body);

    delay(3000);
}


