#include <WiFiUdp.h>
#include "NTPClient.h"
#include "WiFiHelper.h"
#include "PollingTimer.h"

#ifndef WIFI_SSID
#define WIFI_SSID ""  // TODO WiFi SSID (2.4GHz only)
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""  // TODO WiFiパスワード
#endif

WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD);

WiFiUDP udp;

NTPClient timeClient(udp,
                     "ntp.nict.jp", // 日本のNTPサーバー
                     32400, // 日本時間のオフセット
                     86400 * 1000 // 24時間毎にNTPサーバーへアクセスして時間調整させる
);


PollingTimer timer;

void printDateTime() {
    Serial.println(timeClient.getFormattedDate());
}


void setup() {
    Serial.begin(115200);
    delay(3000);

    wifi.on();
    timer.setInterval(printDateTime, 3000, true);
    timer.fire();

}

void loop() {
    timer.handler(); // PollingTimerのイベント用ハンドル
    timeClient.update(); // NTPClientの更新用ハンドル


    delay(10);
}