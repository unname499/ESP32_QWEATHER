#ifndef URL_ENCODE_H
#define URL_ENCODE_H

#include <Arduino.h>

class utils{
    public:
        String urlEncode(const String& input) {
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
};

#endif