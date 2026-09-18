#ifndef QR_TOKEN_H
#define QR_TOKEN_H
#include <string>
#include <ctime>

namespace qrtoken {
    std::string makeToken(const std::string& courseCode, int sessionID, std::time_t expiresAt);
    bool validateToken(const std::string& payload, const std::string& courseCode,
                       int sessionID, std::string& error);
    void printQr(const std::string& payload);
}
#endif