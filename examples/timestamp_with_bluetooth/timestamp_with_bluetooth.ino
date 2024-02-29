/**
 * @file timestamp_with_bluetooth.ino
 * @brief NTPTimeSyncとBluetoothを同時で使うと、クラッシュするためデバッグ用のサンプルコード
 */
//#include "NTPTimeSync.h"
#include "BluetoothSerial.h"
#include "esp_gap_bt_api.h"
#include "DeviceInfo.h"
#include <WiFiUdp.h>
#include "../../NTPClient.h"
#include "../../WiFiHelper.h"
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

BluetoothSerial SerialBT;
esp_bt_pin_code_t pin_code = {1, 2, 3, 4}; // PINコードはAndroidによって機能しない

static void gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

PollingTimer timer;

void printDateTime() {
    Serial.println(timeClient.getFormattedDate());
}

void setup() {
    Serial.begin(115200);

    SerialBT.begin("BTC Device 01"); // Bluetoothデバイス名を設定
    esp_bt_gap_register_callback(gap_callback);

//     Variable PINタイプを使用して、ユーザーがPINコードを入力できるようにする
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE; // 可変のPINコード
    esp_bt_gap_set_pin(pin_type, 0, NULL); // 最初にPINコードを設定しない

    delay(1000);

    wifi.on();
    timer.setInterval(printDateTime, 3000, true);
    timer.fire();

}

void loop() {
    timer.handler(); // PollingTimerのイベント用ハンドル
    timeClient.update(); // NTPClientの更新用ハンドル

    Dev32.printRemainHeap(true);

    delay(5000);
}

static void gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    Serial.print("event: ");
    Serial.println(event);

    switch (event) {
        case ESP_BT_GAP_AUTH_CMPL_EVT:
            Serial.println("認証が完了しました。");
            break;
        case ESP_BT_GAP_PIN_REQ_EVT:
            Serial.println("PINコード認証が要求されました。");
            // PINコードの返信
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
            break;

        default:
            break;
    }
}