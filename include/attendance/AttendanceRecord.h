
#ifndef ATTENDANCE_RECORD_H
#define ATTENDANCE_RECORD_H

#include <string>

// Owner: M2
// Used by: M3 (CourseRepository save/load).
class AttendanceRecord {
private:
    std::string studentID;
    std::string timestamp;
    std::string status;          // "present" or "late"
    std::string captureMethod;

public:
    AttendanceRecord(std::string sID, std::string time, std::string stat, std::string method);

    std::string getStudentID() const;
    std::string getTimestamp() const;
    std::string getStatus() const;
    std::string getCaptureMethod() const;
};

#endif
