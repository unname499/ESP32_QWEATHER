#include <func.h>
#include <time.h>
#include <jwt_ed25519.h>

#define NTP_SERVER "ntp.aliyun.com"  // 阿里云 NTP 服务器，可以自行更换，比如改成time.windows.com
#define GMT_OFFSET_SEC 28800  // UTC+8 北京时间
#define DAYLIGHT_OFFSET_SEC 0  // 夏令时偏移（0 = 不启用）

bool Func::syncTime() {
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

unsigned long Func::getCurrentTimestamp() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        // LOG("[ERR] 获取时间失败");
        Serial.println("WeatherNow.cpp: 无法获取时间");
        return 0;
    }
    return mktime(&timeinfo);
}

String Func::urlEncode(const String& input) {
    if (input.length() == 0) {
        return String();
    }
    
    String output = "";
    output.reserve(input.length() * 3); // 预分配内存，提高效率
    
    for (size_t i = 0; i < input.length(); i++) {
        unsigned char c = (unsigned char)input[i];
        
        // 判断是否需要编码：非字母数字和特定安全字符
        bool need_encode = !(
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~'
        );
        
        if (need_encode) {
            // 格式化为 %XX
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", c);
            output += hex;
        } else {
            output += (char)c;
        }
    }
    
    return output;
}

bool Func::getJWT(String privateKey, String kid, String sub,String* outStr) {
    unsigned long now = Func::getCurrentTimestamp();
    if (now == 0) {
        return false;
    }
    
    unsigned long iat = now - 30;
    unsigned long exp = now + 86400 - 30;
    
    String jwt = generateEd25519JWT(
        privateKey,
        kid,
        sub,
        iat,
        exp
    );
    
    if (jwt.length() == 0) {
        Serial.println("JWT 生成失败");
        return false;
    }

    if (outStr != nullptr) {
        *outStr = jwt;
    }
    return true;
}