/**
 * @file WiFiHelper.cpp
 * @author Toshihiko Arai
 * @brief
 * @version 0.3.3
 * @date 2022-06-28
 *
 */
#include "WiFiHelper.h"

WiFiHelper::WiFiHelper(const char *ssid, const char *password,
                       const char *label, bool enable_event)
    : _ssid(ssid), _password(password), _label(label) {
    using namespace std::placeholders;  // for `_1`
    if (enable_event) {
        _event_id = WiFi.onEvent(std::bind(&WiFiHelper::WiFiEvent, this, _1));
    }
}
/**
 * @brief eventの多重登録を防ぐため
 *
 */
WiFiHelper::~WiFiHelper() { WiFi.removeEvent(_event_id); }

/**
 * @brief virtual関数
 * WiFiHelperをラッパーしてオーバーライドして使う
 *
 */
void WiFiHelper::onDisconnected() {}

/**
 * @brief WiFi.onEventで呼び出されるコールバック関数
 *
 * @param event
 */
void WiFiHelper::WiFiEvent(WiFiEvent_t event) {
    LOG_LN(_label, "Event No:" + String(event));
    switch (event) {
        case SYSTEM_EVENT_WIFI_READY:
            LOG_LN(_label, "WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            LOG_LN(_label, "Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            LOG_LN(_label, "WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            LOG_LN(_label, "WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            LOG_LN(_label, "Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            LOG_LN(_label, "Disconnected from WiFi access point");
            onDisconnected();
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            LOG_LN(_label, "Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            LOG_LN(_label, "My IP address: " + localIP());
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            LOG_LN(_label, "Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            LOG_LN(_label,
                   "WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            ERR_LN(_label,
                   "WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            ERR_LN(_label,
                   "WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            LOG_LN(_label,
                   "WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            LOG_LN(_label, "WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            LOG_LN(_label, "WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            LOG_LN(_label, "Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            LOG_LN(_label, "Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            LOG_LN(_label, "Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            LOG_LN(_label, "Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            LOG_LN(_label, "IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            LOG_LN(_label, "Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            LOG_LN(_label, "Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            LOG_LN(_label, "Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            ERR_LN(_label, "Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            LOG_LN(_label, "Obtained IP address");
            break;
        default:
            break;
    }
}

/**
 * @brief WiFi接続開始
 *
 * @return true -> WiFi接続成功
 * @return false -> SSID、Passwordが未入力 or WiFi接続失敗
 */
bool WiFiHelper::on() {
    if (!checkConfig()) {
        return false;
    }
    /* WIFI_CONNECT_TRY_MAXだけWiFi connect を試す */
    tryConnect();

    return isConnected();
}

/**
 * @brief SSIDとPASSWORDが設定されてるかどうかのチェック
 *
 * @return true
 * @return false
 */
bool WiFiHelper::checkConfig() {
    if (_ssid == nullptr || _password == nullptr) {
        return false;
    } else if (strcmp(_ssid, "") == 0) {
        return false;
    } else if (strcmp(_password, "") == 0) {
        return false;
    }

    return true;
}

/**
 * @brief
 *
 * @return true WiFi接続成功
 * @return false WiFi接続失敗
 */
bool WiFiHelper::isConnected() { return WiFi.status() == WL_CONNECTED; }

void WiFiHelper::connect() {
    WiFi.mode(WIFI_STA);
    delay(500);
    WiFi.begin(_ssid, _password);
}

/**
 * @brief WIFI_CONNECT_TRY_MAXの回数だけconnectを試す
 *
 */
void WiFiHelper::tryConnect() {
    if (!isConnected()) {
        LOG_LN(_label, "Try WiFi connect");
        for (int t = 0; t < WIFI_CONNECT_TRY_MAX; t++) {
            LOG(t + 1);
            connect();

            int count = 0;
            int wait_max = 10;

            while (!isConnected() && (wait_max > count)) {
                LOG(".");
                delay(500);
                count++;
            }
            if (isConnected()) {
                break;
            }
        }
    }
}

/**
 * @brief デバイスのローカルIPアドレス
 *
 * @return String -> ex. 192.168.2.17
 */
String WiFiHelper::localIP() {
    IPAddress myIP = WiFi.localIP();
    return myIP.toString();
}

/**
 * @brief すぐに切断すると送信失敗する可能性あるためdelayを入れてる
 *
 */
void WiFiHelper::off() {
    delay(1000);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
