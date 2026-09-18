#ifndef QR_CODE_CAPTURE_H
#define QR_CODE_CAPTURE_H

#include <string>
#include <ctime>
#include "attendance/AttendanceCapture.h"

// Owner: M2
// Used by: M3 (menu code constructs it, then passes it as AttendanceCapture*).

class QRCodeCapture : public AttendanceCapture {
private:
    std::string courseCode;
    int sessionID;
    std::time_t expiresAt;
    std::string payload;

public:
    QRCodeCapture(std::string courseCode, int sessionID, int durationMins);
    ~QRCodeCapture() override;

    void beginSession() override;
    std::string captureNext() override;
    void endSession() override;
    std::string getMethodName() const override;
};

#endif
