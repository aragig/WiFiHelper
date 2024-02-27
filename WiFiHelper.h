/**
 * @file WiFiHelper.h
 * @author Toshihiko Arai
 * @brief
 * @version 0.3.3
 * @date 2022-06-28
 *
 */
#ifndef WiFiHelper_h
#define WiFiHelper_h
#include <WiFi.h>

#include "Arduino.h"

//#define WIFI_HELPER_PRINT_LOG

#ifdef WIFI_HELPER_PRINT_LOG
#define LOG(...) Serial.print(__VA_ARGS__);
#define LOG_LN(label, ...)  \
    Serial.print(__FILE__); \
    Serial.print(":");      \
    Serial.print(__LINE__); \
    Serial.print(": [");    \
    Serial.print(label);    \
    Serial.print("] ");     \
    Serial.println(__VA_ARGS__);
#else
#define LOG(...)
#define LOG_LN(label, ...)
#endif  // WIFI_HELPER_PRINT_LOG

#ifdef WIFI_HELPER_PRINT_ERROR
#define ERR(...) Serial.print(__VA_ARGS__);
#define ERR_LN(label, ...)  \
    Serial.print(__FILE__); \
    Serial.print(":");      \
    Serial.print(__LINE__); \
    Serial.print(": [");    \
    Serial.print(label);    \
    Serial.print("] ");     \
    Serial.println(__VA_ARGS__);
#else
#define ERR(...)
#define ERR_LN(label, ...)
#endif  // WIFI_HELPER_PRINT_ERROR

class WiFiHelper {
   public:
    WiFiHelper(const char *ssid, const char *password, const char *label,
               bool enable_event = true);
    ~WiFiHelper();
    void WiFiEvent(WiFiEvent_t event);
    bool begin();
    void end();
    bool checkConfig();
    void connect();
    void tryConnect();
    bool isConnected();
    String localIP();
    // void onDisconnected(std::function<void(void)> cb);
    virtual void onDisconnected();

   protected:
    const int WIFI_CONNECT_TRY_MAX = 5;
    const char *_ssid;
    const char *_password;
    const char *_label;
    //    bool _enable_event;
    wifi_event_id_t _event_id;
    // std::function<void(void)> _disconnectedCallback;
};
#endif  // WiFiHelper_h