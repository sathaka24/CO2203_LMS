#ifndef ATTENDANCE_CAPTURE_H
#define ATTENDANCE_CAPTURE_H

#include <string>

// Owner: M2
// Used by: M2 (AttendanceSession, QRCodeCapture), M3 (FileReplayCapture, menu code).
class AttendanceCapture {
public:
    virtual ~AttendanceCapture() = default;

    virtual void beginSession() = 0;
    virtual std::string captureNext() = 0;
    virtual void endSession() = 0;
};

#endif
