#include <Arduino.h>
#include "WiFiHelper.h"

#ifndef WIFI_SSID
#define WIFI_SSID ""  // TODO WiFi SSID (2.4GHz only)
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""  // TODO WiFiパスワード
#endif


WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD, "WiFiHelper");

void setup() {
    Serial.begin(115200);
    delay(3000);
    wifi.begin();
    Serial.println(wifi.localIP());
}

void loop() {
    wifi.begin();
    Serial.println(wifi.localIP());
    delay(3000);
    wifi.end();
    Serial.println(wifi.localIP());
    delay(3000);
}