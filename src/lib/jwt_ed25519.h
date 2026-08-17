#ifndef JWT_ED25519_H
#define JWT_ED25519_H

#include <Arduino.h>
#include <mbedtls/base64.h>
#include "Ed25519.h"   // C++ 类

// -------- Base64URL 编码 --------
inline String base64UrlEncode(const uint8_t* data, size_t len) {
    size_t outLen = 0;
    mbedtls_base64_encode(NULL, 0, &outLen, data, len);
    
    char* buffer = (char*)malloc(outLen + 1);
    if (!buffer) return "";
    
    mbedtls_base64_encode((unsigned char*)buffer, outLen + 1, &outLen, data, len);
    
    String result = "";
    for (size_t i = 0; i < outLen; i++) {
        char c = buffer[i];
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
        else if (c == '=') continue;
        result += c;
    }
    free(buffer);
    return result;
}

// -------- 从 PEM 提取 Ed25519 私钥 --------
inline bool extractEd25519PrivateKey(const String& pemKey, uint8_t* outPrivateKey) {
    String keyData = pemKey;
    keyData.replace("-----BEGIN PRIVATE KEY-----", "");
    keyData.replace("-----END PRIVATE KEY-----", "");
    keyData.replace("\n", "");
    keyData.replace("\r", "");
    keyData.trim();
    
    if (keyData.length() == 0) {
        Serial.println("[ERR] 私钥数据为空");
        return false;
    }
    
    size_t decodedLen = 0;
    mbedtls_base64_decode(NULL, 0, &decodedLen,
                          (const unsigned char*)keyData.c_str(), keyData.length());
    
    if (decodedLen == 0) {
        Serial.println("[ERR] Base64 解码失败");
        return false;
    }
    
    unsigned char* decoded = (unsigned char*)malloc(decodedLen);
    if (!decoded) {
        Serial.println("[ERR] 内存分配失败");
        return false;
    }
    
    int ret = mbedtls_base64_decode(decoded, decodedLen, &decodedLen,
                                    (const unsigned char*)keyData.c_str(), keyData.length());
    
    if (ret != 0) {
        Serial.printf("[ERR] Base64 解码错误: %d\n", ret);
        free(decoded);
        return false;
    }
    
    if (decodedLen < 32) {
        Serial.printf("[ERR] 解码后数据太短: %d 字节 (需要 >= 32)\n", decodedLen);
        free(decoded);
        return false;
    }
    
    // Ed25519 私钥在 DER 中通常是最后 32 字节
    memcpy(outPrivateKey, decoded + decodedLen - 32, 32);
    free(decoded);
    return true;
}

// -------- 生成 Ed25519 JWT --------
inline String generateEd25519JWT(const String& privateKeyPEM,
                                 const String& kid,
                                 const String& sub,
                                 unsigned long iat,
                                 unsigned long exp) {
    // 1. 构建 Header
    String header = "{";
    header += "\"alg\":\"EdDSA\",";
    header += "\"kid\":\"" + kid + "\"";
    header += "}";
    
    // 2. 构建 Payload
    String payload = "{";
    payload += "\"iat\":" + String(iat) + ",";
    payload += "\"exp\":" + String(exp) + ",";
    payload += "\"sub\":\"" + sub + "\"";
    payload += "}";
    
    // 3. Base64URL 编码
    String headerB64 = base64UrlEncode((const uint8_t*)header.c_str(), header.length());
    String payloadB64 = base64UrlEncode((const uint8_t*)payload.c_str(), payload.length());
    String signingInput = headerB64 + "." + payloadB64;
    
    // 4. 提取私钥
    uint8_t privateKey[32];
    if (!extractEd25519PrivateKey(privateKeyPEM, privateKey)) {
        Serial.println("[ERR] 私钥提取失败");
        return "";
    }
    
    // 5. 从私钥派生公钥（使用 C++ 类的静态方法）
    uint8_t publicKey[32];
    Ed25519::derivePublicKey(publicKey, privateKey);   // ← 改成这个
    
    // 6. 执行 Ed25519 签名（使用 C++ 类的静态方法）
    uint8_t signature[64];
    Ed25519::sign(signature, privateKey, publicKey,    // ← 改成这个
                  (const uint8_t*)signingInput.c_str(),
                  signingInput.length());
    
    // 7. 组合最终 JWT
    String signatureB64 = base64UrlEncode(signature, 64);
    return signingInput + "." + signatureB64;
}

#endif  // JWT_ED25519_H