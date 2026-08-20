#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "weatherNow.h"

#define WIFI_SSID "TP-LINK_16F3"
#define WIFI_PASSWD "qwer123tyui456"

const char* PRIVATE_KEY_PEM = R"(
-----BEGIN PRIVATE KEY-----
MC4CAQAwBQYDK2VwBCIEIKo45LPn7Vg7bfscVbVkenFMuiTPkRXI7d/EdeNkChNw
-----END PRIVATE KEY-----
)";

const char* KID = "TAGXVWVUAB";
const char* SUB = "3G878JBYCK";

// 创建全局 Config 对象（location 使用中文，http 请求时会自动 URL 编码）
Config gc(PRIVATE_KEY_PEM, KID, SUB, 
          "nw6vhhehnj.re.qweatherapi.com", "", "泰和");

// 将 Config 传递给 WeatherNow
WeatherNow weatherNow(gc);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== SETUP START ===");

    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    Serial.print("连接 WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" 成功");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    if (weatherNow.get()) {
        Serial.println("\n====== Weather Now Info ======");
        Serial.print("Server Response: ");
        Serial.println(weatherNow.getServerCode());
        Serial.print("Temperature: ");
        Serial.println(weatherNow.getTemp());
        Serial.print("FeelsLike: ");
        Serial.println(weatherNow.getFeelLike());
        Serial.print("Weather Now: ");
        Serial.println(weatherNow.getWeatherText());
        Serial.print("WindDir: ");
        Serial.println(weatherNow.getWindDir());
        Serial.print("WindScale: ");
        Serial.println(weatherNow.getWindScale());
        Serial.print("Humidity: ");
        Serial.println(weatherNow.getHumidity());
        Serial.print("Precip: ");
        Serial.println(weatherNow.getPrecip());
        Serial.println("==============================");
    } else {
        Serial.println("Update Failed...");
        Serial.print("Server Response: ");
        Serial.println(weatherNow.getServerCode());
    }
}

void loop() {
    delay(10000);
}