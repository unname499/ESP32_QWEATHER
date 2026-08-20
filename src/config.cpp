#include "config.h"
#include <time.h>
#include "jwt_ed25519.h"

#define NTP_SERVER "ntp.aliyun.com"
#define GMT_OFFSET_SEC 28800
#define DAYLIGHT_OFFSET_SEC 0

// 构造函数
Config::Config(String privateKey, String kid, String sub, 
               String apiHost, String key, String location)
    : _privateKey(privateKey), _kid(kid), _sub(sub),
      _apiHost(apiHost), _key(key), _location(location) {}

// 静态方法实现
bool Config::syncTime() {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    Serial.print("[INFO] 同步 NTP 时间");
    int attempts = 0;
    while (time(nullptr) < 100000 && attempts < 60) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (time(nullptr) > 100000) {
        Serial.println(" 成功");
        return true;
    }
    Serial.println(" 失败");
    return false;
}

unsigned long Config::getCurrentTimestamp() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Config: 无法获取时间");
        return 0;
    }
    return mktime(&timeinfo);
}

String Config::urlEncode(const String& input) {
    if (input.length() == 0) return String();

    String output = "";
    output.reserve(input.length() * 3);

    for (size_t i = 0; i < input.length(); i++) {
        unsigned char c = (unsigned char)input[i];
        bool need_encode = !(
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~'
        );
        if (need_encode) {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", c);
            output += hex;
        } else {
            output += (char)c;
        }
    }
    return output;
}

bool Config::getJWT(const String& privateKey, const String& kid, 
                    const String& sub, String* outStr) {
    unsigned long now = getCurrentTimestamp();
    if (now == 0) return false;

    unsigned long iat = now - 30;
    unsigned long exp = now + 86400 - 30;

    String jwt = generateEd25519JWT(privateKey, kid, sub, iat, exp);
    if (jwt.length() == 0) {
        Serial.println("JWT 生成失败");
        return false;
    }
    if (outStr != nullptr) {
        *outStr = jwt;
    }
    return true;
}