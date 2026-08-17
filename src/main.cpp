#include <Arduino.h>
#include <weather.h>
#include <WiFi.h>

const char* PRIVATE_KEY_PEM = R"(
-----BEGIN PRIVATE KEY-----
MC4CAQAwBQYDK2VwBCIEIKo45LPn7Vg7bfscVbVkenFMuiTPkRXI7d/EdeNkChNw
-----END PRIVATE KEY-----
)";

const char* KID = "TAGXVWVUAB";
const char* SUB = "3G878JBYCK";

WeatherNow weatherNow;

void setup(){
    Serial.begin(115200);
    WiFi.begin("TP-LINK_16F3", "qwer123tyui456");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
    }

    weatherNow.config(PRIVATE_KEY_PEM, KID, "nw6vhhehnj.re.qweatherapi.com"
        , "", SUB, Func::urlEncode("泰和"));
    IPAddress ip;
    if (WiFi.hostByName("nw6vhhehnj.re.qweatherapi.com", ip)) {
        Serial.print("DNS resolved: ");
        Serial.println(ip);
    } else {
        Serial.println("DNS failed");
    }
    if(weatherNow.get()){ // 获取天气更新
    Serial.println(F("======Weahter Now Info======"));
    Serial.print("Server Response: ");
    Serial.println(weatherNow.getServerCode());  // 获取API状态码
    Serial.print(F("Temperature: "));
    Serial.println(weatherNow.getTemp());        // 获取实况温度
    Serial.print(F("FeelsLike: "));
    Serial.println(weatherNow.getFeelLike());    // 获取实况体感温度
    Serial.print(F("Weather Now: "));
    Serial.println(weatherNow.getWeatherText()); // 获取实况天气状况的文字描述
    Serial.print(F("windDir: "));
    Serial.println(weatherNow.getWindDir());     // 获取实况风向
    Serial.print(F("WindScale: "));
    Serial.println(weatherNow.getWindScale());   // 获取实况风力等级
    Serial.print(F("Humidity: "));
    Serial.println(weatherNow.getHumidity());    // 获取实况相对湿度百分比数值
    Serial.print(F("Precip: "));
    Serial.println(weatherNow.getPrecip());      // 获取实况降水量,毫米
    Serial.println(F("========================"));
  } else {    // 更新失败
    Serial.println("Update Failed...");
    Serial.print("Server Response: ");
    Serial.println(weatherNow.getServerCode());
  }
}

void loop(){
    delay(1000000);
}