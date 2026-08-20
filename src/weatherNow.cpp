#include "weatherNow.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoUZlib.h>

// #define DEBUG

WeatherNow::WeatherNow(const Config& config) : _config(config) {
    _client.setInsecure();
    _client.setTimeout(15000);
}

bool WeatherNow::getGeo() {
    // 1. 同步时间
    if (!_config.syncTime()) {
        #ifdef DEBUG
        Serial.println("时间同步失败");
        #endif
        return false;
    }

    // 2. 生成 JWT
    String jwt;
    if (!_config.getJWT(_config.getPrivateKey(), _config.getKID(), 
                         _config.getSUB(), &jwt)) {
        #ifdef DEBUG
        Serial.println("JWT 生成失败");
        #endif
        return false;
    }

    // 3. 对 location 进行 URL 编码
    String encodedLocation = _config.urlEncode(_config.getLocation());

    // 4. 构造请求
    HTTPClient http;
    String url = "https://" + _config.getApiHost() + 
                 "/geo/v2/city/lookup?location=" + encodedLocation;

    #ifdef DEBUG
    Serial.println("[HTTP] begin...");
    #endif

    if (!http.begin(_client, url)) {
        #ifdef DEBUG
        Serial.println("HTTP begin 失败");
        #endif
        return false;
    }

    http.addHeader("Authorization", "Bearer " + jwt);
    http.addHeader("Accept-Encoding", "gzip, deflate");

    #ifdef DEBUG
    Serial.println("[HTTP] GET...");
    #endif

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        WiFiClient* stream = http.getStreamPtr();
        int size = http.getSize();
        uint8_t inbuff[size];
        stream->readBytes(inbuff, size);

        uint8_t* outbuf = NULL;
        uint32_t out_size = 0;
        int result = ArduinoUZlib::decompress(inbuff, size, outbuf, out_size);
        String payload = String(outbuf, out_size);

        _parseGeoJson(payload);

        #ifdef DEBUG
        Serial.println(payload);
        #endif

        free(outbuf);
        http.end();
        return true;
    } else {
        #ifdef DEBUG
        Serial.printf("HTTP 错误: %d\n", httpCode);
        #endif
        http.end();
        return false;
    }
}

bool WeatherNow::get() {
    if (!getGeo()) {
        Serial.println("Geo API 请求失败");
        return false;
    }

    String jwt;
    if (!_config.getJWT(_config.getPrivateKey(), _config.getKID(), 
                         _config.getSUB(), &jwt)) {
        Serial.println("JWT 生成失败");
        return false;
    }

    String url = "https://" + _config.getApiHost() + 
                 "/weather/v1/current/" + _lat + "/" + _lon;

    HTTPClient http;
    if (!http.begin(_client, url)) {
        return false;
    }

    http.addHeader("Authorization", "Bearer " + jwt);
    http.addHeader("Accept-Encoding", "gzip, deflate");

    int httpCode = http.GET();
    _response_code = String(httpCode);

    if (httpCode == HTTP_CODE_OK) {
        WiFiClient* stream = http.getStreamPtr();
        int size = http.getSize();
        uint8_t inbuff[size];
        stream->readBytes(inbuff, size);

        uint8_t* outbuf = NULL;
        uint32_t out_size = 0;
        int result = ArduinoUZlib::decompress(inbuff, size, outbuf, out_size);
        String payload = String(outbuf, out_size);

        _parseNowJson(payload);

        #ifdef DEBUG
        Serial.println(payload);
        #endif

        free(outbuf);
        http.end();
        return true;
    } else {
        #ifdef DEBUG
        Serial.printf("HTTP 错误: %d\n", httpCode);
        #endif
        http.end();
        return false;
    }
}

void WeatherNow::_parseGeoJson(String payload) {
    JsonDocument doc;
    deserializeJson(doc, payload);
    _lat = doc["location"][0]["lat"].as<String>();
    _lon = doc["location"][0]["lon"].as<String>();
}

void WeatherNow::_parseNowJson(String payload) {
    JsonDocument doc;
    deserializeJson(doc, payload);

    _now_temp_float = doc["temperature"]["value"].as<float>();
    _now_feelsLike_float = doc["feelsLike"]["value"].as<float>();
    _now_weather_code_int = doc["condition"]["code"].as<int>();
    _now_text_str = doc["condition"]["text"].as<String>();
    _now_windDir_str = doc["wind"]["direction"]["compass"].as<String>();
    _now_windScale_int = doc["wind"]["scale"].as<int>();
    _now_humidity_float = doc["humidity"].as<float>();
    _now_precip_float = doc["precipitation"]["intensity"]["value"].as<float>();
}

String WeatherNow::getServerCode() { return _response_code; }
float WeatherNow::getTemp() { return _now_temp_float; }
float WeatherNow::getFeelLike() { return _now_feelsLike_float; }
String WeatherNow::getWeatherText() { return _now_text_str; }
String WeatherNow::getWindDir() { return _now_windDir_str; }
int WeatherNow::getWindScale() { return _now_windScale_int; }
float WeatherNow::getHumidity() { return _now_humidity_float; }
float WeatherNow::getPrecip() { return _now_precip_float; }