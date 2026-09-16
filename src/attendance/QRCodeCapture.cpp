#include "attendance/QRCodeCapture.h"
#include <iostream>

QRCodeCapture::QRCodeCapture(std::string session, int expiry, std::string hash)
    : sessionID(std::move(session)), expiryTime(expiry), integrityHash(std::move(hash)) {}

QRCodeCapture::~QRCodeCapture() = default;

bool QRCodeCapture::validateIntegrity(std::string payload) const {
    // Valid payload format: <studentID>:<hash>
    size_t colonPos = payload.find(':');
    if (colonPos == std::string::npos) {
        return false;
    }
    std::string tokenHash = payload.substr(colonPos + 1);
    return (tokenHash == integrityHash);
}

void QRCodeCapture::beginSession() {
    std::cout << "\n=======================================================\n";
    std::cout << " [QR CODE ATTENDANCE SESSION INITIALIZED]\n";
    std::cout << " Session ID     : " << sessionID << "\n";
    std::cout << " Expiry Duration: " << expiryTime << " minutes\n";
    std::cout << " Integrity Token: " << integrityHash << "\n";
    std::cout << " Student Payload: <StudentID>:" << integrityHash << "\n";
    std::cout << "=======================================================\n";
}

std::string QRCodeCapture::captureNext() {
    std::string inputPayload;
    std::cout << "Submit scanned QR Payload (or type 'EXIT' to finish): ";
    std::cin >> inputPayload;

    if (inputPayload == "EXIT" || inputPayload == "exit") {
        return "";
    }

    if (!validateIntegrity(inputPayload)) {
        std::cout << " [ERROR] Invalid QR Code integrity hash / expired token!\n";
        return "INVALID_TOKEN";
    }

    // Extract studentID from <studentID>:<hash>
    return inputPayload.substr(0, inputPayload.find(':'));
}

void QRCodeCapture::endSession() {
    std::cout << " [QR CODE SESSION CLOSED]\n";
}