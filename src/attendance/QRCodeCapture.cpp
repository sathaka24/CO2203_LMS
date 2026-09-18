#include "attendance/QRCodeCapture.h"
#include "attendance/QrToken.h"
#include <iostream>

QRCodeCapture::QRCodeCapture(std::string courseCode, int sessionID, int durationMins)

    : courseCode(std::move(courseCode)), sessionID(sessionID), expiresAt(std::time(nullptr) + durationMins * 60) {

        payload = qrtoken::makeToken(courseCode, sessionID, expiresAt);
    }

QRCodeCapture::~QRCodeCapture() = default;

void QRCodeCapture::beginSession() {
    std::cout << "\n=======================================================\n";
    std::cout << "\n Scan this code for " << courseCode << " session #" << sessionID << "\n\n";
    qrtoken::printQr(payload);
    std::cout << "\n Students: scan the code, then enter <studentID> <payload> below.\n";
    std::cout << "=======================================================\n";
}

std::string QRCodeCapture::captureNext() {

    std::string studentID;
    std::string submitted;

    std::cout << "Student ID and payload (or EXIT): ";

    std::cin >> studentID;

    if (studentID == "EXIT" || studentID == "exit") return "";

    std::cin >> submitted;

    std::string error;

    if (!qrtoken::validateToken(submitted, courseCode, sessionID, error)) {

        std::cout << " [REJECTED] " << error << "\n";
        return "INVALID_TOKEN";
    }
    return studentID;
}

void QRCodeCapture::endSession() {
    std::cout << " [QR CODE SESSION CLOSED]\n";
}

std::string QRCodeCapture::getMethodName() const {
    return "QR_CODE";
}