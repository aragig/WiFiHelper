# WiFiHelper Library Simplifying WiFi Connection on ESP32


[TOC]

## Introduction

日本語の解説ページ: https://101010.fun/programming/arduino-wifi-helper.html

- **Writing WiFi code every time is a hassle**
- **I want to retry connecting when WiFi connection fails**
- **I want to keep the main program clean by managing WiFi connections with a library**
- **I want detailed error messages when WiFi connection fails**

To address these issues I faced personally, I created the `WiFiHelper` library, which I will introduce here.

You can describe WiFi connections easily with the following image.
```cpp
WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD);

...

loop() {
    wifi.on()
    // During this time, perform HTTP requests with an HTTP client, etc.
    wifi.off()
}
```

## Operating Environment

It operates on the ESP32. However, the firmware for the ESP32 must be [2.0.2](https://github.com/espressif/arduino-esp32#2.0.2). Operation on other versions has not been confirmed.

## Library Installation
The source code is available in this repository.

It is not published as an Arduino library or a PlatformIO library, so please clone it directly from the GitHub repository or download it as a zip file. `WiFiHelper.h` and `WiFiHelper.cpp` are the two source code files for this library. Please place them correctly in your project's library directory.

As of February 2024, version 0.3.4 is available. This `WiFiHelper` has been used in my work as well, and it has been operating stably without any problems so far.

### Installing with platformio.ini
If you are using PlatformIO, you can install `WiFiHelper` by adding the repository URL to the `lib_deps` item in your `platformio.ini`.

```ini:platformio.ini
[platformio]
;src_dir = lib/xxxxx/examples/xxxxxx

[env]
platform = https://github.com/platformio/platform-espressif32.git
board = esp32dev
framework = arduino
monitor_speed = 115200

platform_packages =
    framework-arduinoespressif32 @ https://github.com/espressif/arduino-esp32#2.0.2

[env:debug]
lib_deps = https://github.com/aragig/WiFiHelper.git
build_flags =
    -DWIFI_SSID=\"${sysenv.WIFI_SSID}\"
    -DWIFI_PASSWORD=\"${sysenv.WIFI_PASSWORD}\"
    -DWIFI_HELPER_PRINT_LOG
    -DWIFI_HELPER_PRINT_ERROR

```

By the way, `${sysenv.WIFI_SSID}` and `${sysenv.WIFI_PASSWORD}` are ways to import information from shell environment variables. Writing the WiFi information in advance in `~/.zsrch` or similar can be a security measure.

## How to Use
Here, I will introduce one of the sample codes found in the `examples` folder and explain how to use it.

### Source Code (ESP32)
This is the source code for running on the ESP32. It is a program that establishes a WiFi connection using `WiFiHelper`, sends a simple HTTP request, and receives data.

```cpp:http_request.ino
#include <Arduino.h>
#include "WiFiHelper.h"

#ifndef WIFI_SSID
#define WIFI_SSID ""  // TODO WiFi SSID (2.4GHz only)
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""  // TODO WiFi password
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

    if (!wifi.on()) { // WiFi connection
        response.code = -99;
        response.body = "Failed to initialize WiFi";
        return response; // Failed to initialize WiFi
    }

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, url)) {
        response.code = -98;
        response.body = "Failed HTTPClient begin!";
        return response; // Failed HTTP client initialization
    }

    http.addHeader("Content-Type", "application/json"); // Specify content type

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        // If there is a normal response, get the server's response (if necessary)
        response.code = httpResponseCode;
        response.body = http.getString();
    } else {
    }

    http.end(); // End HTTP connection
    wifi.off(); // Disconnect WiFi
    return response;
}

void setup() {
    Serial.begin(115200);
    delay(10000);
}

void loop() {
    String url = "http://apppppp.com/jojo.json";

    Serial.println("HTTP

 Request... ");

    HttpResponse res = http_request(url);
    Serial.print("HTTP Response code: "); Serial.println(res.code);
    Serial.print("HTTP Response body: ");
    Serial.println(res.body);

    delay(3000);
}
```

### Explanation

### Initialization, Instance Creation
`#include "WiFiHelper.h"` to include the library, and `WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD, "WiFiHelper");` to set up the WiFi connection information. The string literal "WiFiHelper" serves as a debug label. This is provided to make it easier to debug when creating classes that inherit from `WiFiHelper`, allowing each instance to have its own label.

The `http_request` function uses `WiFiHelper`. `wifi.on()` attempts to establish a WiFi connection. As you can see in the source code of `WiFiHelper.cpp`, WiFi connections may fail to establish on the first try, so it retries up to 5 times by default. This number can be changed in `WiFiHelper.h` with `WIFI_CONNECT_TRY_MAX`.

During the operation of the ESP32, the WiFi may get disconnected for some reason. In such cases, just calling `WiFi.on()` is sufficient. Therefore, it is safer to call it in the `loop()` event before the HTTP request, as shown in the source code, rather than calling it in the `setup`.

Also, `wifi.off()` is used to end the WiFi connection, but if you want to keep the WiFi connected at all times, you can omit this process.

Finally, `WiFiHelper` implements hooks for WiFi connection events to output messages. If you define `WIFI_HELPER_PRINT_LOG` or `WIFI_HELPER_PRINT_ERROR` with `#define`, it should be helpful for debugging.

