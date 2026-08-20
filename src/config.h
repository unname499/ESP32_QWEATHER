#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

class Config {
    private:
        String _apiHost;
        String _key;
        String _kid;
        String _sub;
        String _privateKey;
        String _location;

    public:
        // 构造函数
        Config(String privateKey, String kid, String sub, 
               String apiHost, String key, String location);

        // Getter 方法（只读）
        String getApiHost() const { return _apiHost; }
        String getKey() const { return _key; }
        String getKID() const { return _kid; }
        String getSUB() const { return _sub; }
        String getPrivateKey() const { return _privateKey; }
        String getLocation() const { return _location; }

        // 静态工具方法
        static bool syncTime();
        static unsigned long getCurrentTimestamp();
        static String urlEncode(const String& input);
        static bool getJWT(const String& privateKey, const String& kid, 
                           const String& sub, String* outStr);
};

#endif