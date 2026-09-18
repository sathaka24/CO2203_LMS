#include "attendance/QrToken.h"
#include "third_party/qrcodegen.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

using qrcodegen::QrCode;


namespace {

    const std::string SECRET = "CO2203-ENCODERS"; 

    // below function we use to calculate a hash value. which is the token we shows in qr code
    std::string hashOf(const std::string& data) {          // FNV-1a

        unsigned long long h = 1469598103934665603ULL;

        for (unsigned char ch : data) { 

            h ^= ch; 
            
            h *= 1099511628211ULL; 
        
        }

        std::ostringstream os;
        os << std::hex << std::setw(16) << std::setfill('0') << h;

        return os.str();
    }

    // below function use to split a given string using given char
    std::vector<std::string> split(const std::string& s, char sep) {

        std::vector<std::string> out; 
        std::string part; 
        std::istringstream is(s);

        while (std::getline(is, part, sep)) {
            
            out.push_back(part);

        }

        return out;
    }
}

// below namespace wrapped up some aux function need when dealing with qr code
namespace qrtoken {

// this create a token
std::string makeToken(const std::string& courseCode, int sessionID, std::time_t expiresAt) {

    std::ostringstream body;

    body << courseCode << '|' << sessionID << '|' << expiresAt;

    return body.str() + '|' + hashOf(SECRET + body.str());

}

// this validate the token
bool validateToken(const std::string& payload, const std::string& courseCode,int sessionID, std::string& error) {

    std::vector<std::string> f = split(payload, '|');

    if (f.size() != 4) { 

        error = "payload must look like COURSE|SESSION|EXPIRY|HASH";
        return false; 
    }

    std::ostringstream body;
    body << f[0] << '|' << f[1] << '|' << f[2];

    if (hashOf(SECRET + body.str()) != f[3]) {
        
        error = "integrity hash does not match";
        return false;
    
    }
    if (f[0] != courseCode || f[1] != std::to_string(sessionID)) {
        
        error = "token is for a different session";
        
        return false;
    
    }

    std::time_t expiry = 0;
    try { 

        expiry = static_cast<std::time_t>(std::stoll(f[2]));
    
    }
    catch (const std::exception&) { 
        
        error = "expiry is not a number";
        return false;
    
    }
    if (std::time(nullptr) > expiry) {
        
        error = "token has expired";
        return false;
    
    }

    return true;
}

// this is the function which print the qr code on terminal
void printQr(const std::string& payload) {

    const QrCode qr = QrCode::encodeText(payload.c_str(), QrCode::Ecc::MEDIUM);


    const int border = 2;

    for (int y = -border; y < qr.getSize() + border; ++y) {

        for (int x = -border; x < qr.getSize() + border; ++x)

            if (qr.getModule(x, y))
            {
                std::cout << "\033[40m  \033[0m";
            } else {

                std::cout <<  "\033[47m  \033[0m";
            }

        std::cout << "\n";
    }

    
    std::cout << "Payload: " << payload << "\n";
}

}