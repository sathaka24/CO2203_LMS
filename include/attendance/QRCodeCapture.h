#ifndef QR_CODE_CAPTURE_H
#define QR_CODE_CAPTURE_H

#include <string>
#include "attendance/AttendanceCapture.h"

// Owner: M2
// Used by: M3 (menu code constructs it, then passes it as AttendanceCapture*).

class QRCodeCapture : public AttendanceCapture {
private:
    std::string sessionID;
    int expiryTime;
    std::string integrityHash;

    bool validateIntegrity(std::string payload) const;

public:
    QRCodeCapture(std::string sessionID, int expiry, std::string hash);
    ~QRCodeCapture() override;

    void beginSession() override;
    std::string captureNext() override;
    void endSession() override;
};

#endif
