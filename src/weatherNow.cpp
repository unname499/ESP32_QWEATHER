#include <weatherNow.h>
#include <jwt_ed25519.h>
#include <func.h>
// #define DEBUG


void WeatherNow::config(String privateKey, String kid, String apiHost, String key, 
            String sub, String location){
    _requserPrivateKey = privateKey;
    _kid = kid;
    _sub = sub;
    _apiHost = apiHost;
    _key = key;
    _reqLocation = location;
    _client.setInsecure();
    _client.setTimeout(15000);
}

bool WeatherNow::getGeo(){
    if(!Func::syncTime()){
        #ifdef DEBUG
        Serial.println("时间同步失败");
        #endif
        return false;
    }
    if(!Func::getJWT(_requserPrivateKey , _kid, _sub, &JWT)){
        #ifdef DEBUG
        Serial.println("JWT 获取失败");
        #endif
        return false;
    }
    String url;
    HTTPClient http;
    if(_key == ""){
        url = "https://" + _apiHost + "/geo/v2/city/lookup?location=" + _reqLocation;
        http.addHeader("Authorization", "Bearer " + JWT);
        http.addHeader("Accept-Encoding", "gzip, deflate");
    }else{
        url = "https://" + _apiHost + "/geo/v2/city/lookup?location=" + _reqLocation + 
        "&key=" + _key;
    }
#ifdef DEBUG
    Serial.print("[HTTP] begin...\n");
#endif
    if (http.begin(_client, url))
    {
        #ifdef DEBUG
        Serial.println("HTTPclient setUp done!");
        #endif
    }
#ifdef DEBUG
    Serial.print("[HTTP] GET...\n");
#endif
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
#ifdef DEBUG
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
#endif
        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            WiFiClient *stream = http.getStreamPtr();
            int size = http.getSize();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);
            uint8_t *outbuf = NULL;
            uint32_t out_size = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, out_size);
            String payload = String(outbuf, out_size);
            _parseGeoJson(payload);
            #ifdef DEBUG
            Serial.println(payload);
            #endif
        }
    }
    else
    {
#ifdef DEBUG
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
#endif
        return false;
    }

    http.end();
    return true;
}

bool WeatherNow::get(){
    if(!WeatherNow::getGeo()){
        Serial.println("Geo API请求失败。");
        return false;
    }
    String url;
    HTTPClient http;
    if(_key == ""){
        url = "https://" + _apiHost + "/weather/v1/current/" + _lat + "/" + _lon;
        http.addHeader("Authorization", "Bearer " + JWT);
        http.addHeader("Accept-Encoding", "gzip, deflate");
    }else{
        url = "https://" + _apiHost + "/weather/v1/current/" + _lat + "/" + _lon + 
        "?key=" + _key;
    }
#ifdef DEBUG
    Serial.print("[HTTP] begin...\n");
#endif
    if (http.begin(_client, url))
    {
        #ifdef DEBUG
        Serial.println("HTTPclient setUp done!");
        #endif
    }
#ifdef DEBUG
    Serial.print("[HTTP] GET...\n");
#endif
    // start connection and send HTTP header
    int httpCode = http.GET();
    _response_code = httpCode;
    // httpCode will be negative on error
    if (httpCode > 0)
    {
#ifdef DEBUG
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
#endif
        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            WiFiClient *stream = http.getStreamPtr();
            int size = http.getSize();
            uint8_t inbuff[size];
            stream->readBytes(inbuff, size);
            uint8_t *outbuf = NULL;
            uint32_t out_size = 0;
            int result = ArduinoUZlib::decompress(inbuff, size, outbuf, out_size);
            String payload = String(outbuf, out_size);
            _parseNowJson(payload);
            #ifdef DEBUG
            Serial.println(payload);
            #endif
        }
    }
    else
    {
#ifdef DEBUG
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
#endif
        return false;
    }

    http.end();
    return true;
}

void WeatherNow::_parseGeoJson(String payload){
    JsonDocument doc;

    deserializeJson(doc, payload);

    _lat = doc["location"][0]["lat"].as<String>();
    _lon = doc["location"][0]["lon"].as<String>();
}

void WeatherNow::_parseNowJson(String payload){
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

String WeatherNow::getServerCode()
{
    return _response_code;
}

// 实况温度
float WeatherNow::getTemp()
{
    return _now_temp_float;
}

// 实况体感温度
float WeatherNow::getFeelLike()
{
    return _now_feelsLike_float;
}

// 实况天气状况的文字描述
String WeatherNow::getWeatherText()
{
    return _now_text_str;
}

// 实况风向
String WeatherNow::getWindDir()
{
    return _now_windDir_str;
}

// 实况风力等级
int WeatherNow::getWindScale()
{
    return _now_windScale_int;
}

// 实况相对湿度百分比数值
float WeatherNow::getHumidity()
{
    return _now_humidity_float;
}
// 实况降水量,毫米
float WeatherNow::getPrecip()
{
    return _now_precip_float;
}