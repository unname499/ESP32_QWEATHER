#ifndef _QWEATHER_NOW_H_
#define _QWEATHER_NOW_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoUZlib.h>

class WeatherNow{
    public:
        void config(String privateKey, String kid, String apiHost, String key = "", 
            String sub = "", String location = "");

        bool get();
        String getServerCode();
        float getTemp();
        float getFeelLike();
        String getWeatherText();
        String getWindDir();
        int getWindScale();
        float getHumidity();
        float getPrecip();
        static String JWT;
        static String _key; // API KEY 传统方式验证
        static String _apiHost; // 自己的api host
        static String _kid;  // 项目 kid
        static String _sub;  // 项目 sub
        static String _requserPrivateKey;  // 私钥
        static String _reqLocation; // 位置
        static String _lat;
        static String _lon;

  private:
    WiFiClientSecure _client;
    const int httpsPort = 443;
    

    void _parseNowJson(String payload);  // 解析json信息
    void _parseGeoJson(String payload);  // 解析json信息
    bool getGeo();

    String _response_code =  "no_init";  // API状态码
    float _now_temp_float = 999.00;             // 实况温度
    float _now_feelsLike_float = 999.00;       // 实况体感温度
    int _now_weather_code_int = 999;             // 当前天气状况和图标的代码
    String _now_text_str = "no_init";    // 实况天气状况的文字描述
    String _now_windDir_str = "no_init"; // 实况风向
    int _now_windScale_int = 999;        // 实况风力等级
    float _now_humidity_float = 999;     // 实况相对湿度百分比数值
    float _now_precip_float = 999;       // 实况降水量,毫米
};

#endif