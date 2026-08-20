#ifndef WEATHERNOW_H
#define WEATHERNOW_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "config.h"

class WeatherNow {
    private:
        const Config& _config;
        WiFiClientSecure _client;

        String _lat;
        String _lon;
        String _response_code;

        float _now_temp_float;
        float _now_feelsLike_float;
        int _now_weather_code_int;
        String _now_text_str;
        String _now_windDir_str;
        int _now_windScale_int;
        float _now_humidity_float;
        float _now_precip_float;

        bool getGeo();
        void _parseGeoJson(String payload);
        void _parseNowJson(String payload);

    public:
        WeatherNow(const Config& config);
        bool get();
        String getServerCode();
        float getTemp();
        float getFeelLike();
        String getWeatherText();
        String getWindDir();
        int getWindScale();
        float getHumidity();
        float getPrecip();
};

#endif