#ifndef _FUNC_H
#define _FUNC_H

#include <Arduino.h>

class Func{
    public:
        static bool syncTime();
        static unsigned long getCurrentTimestamp();
        static String urlEncode(const String& input);
        static bool getJWT(String privateKey, String kid, String sub,String* outStr);
};

#endif