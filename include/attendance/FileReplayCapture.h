#ifndef FILE_REPLAY_CAPTURE_H
#define FILE_REPLAY_CAPTURE_H

#include <string>
#include <fstream>
#include "attendance/AttendanceCapture.h"

// Owner: M3  (Spec Section 5 assigns FileReplayCapture to Member 3)
// Used by: M3 (menu code constructs it, then passes it as AttendanceCapture*).

class FileReplayCapture : public AttendanceCapture {
private:
    std::string filePath;
    std::ifstream fileStream;

public:
    explicit FileReplayCapture(std::string path);
    ~FileReplayCapture() override;

    void beginSession() override;
    std::string captureNext() override;
    void endSession() override;
};

#endif
