#include "attendance/AttendanceRecord.h"

AttendanceRecord::AttendanceRecord(std::string sID, std::string time, std::string stat, std::string method)
    : studentID(std::move(sID)), timestamp(std::move(time)), 
      status(std::move(stat)), captureMethod(std::move(method)) {}

std::string AttendanceRecord::getStudentID() const { return studentID; }
std::string AttendanceRecord::getTimestamp() const { return timestamp; }
std::string AttendanceRecord::getStatus() const { return status; }
std::string AttendanceRecord::getCaptureMethod() const { return captureMethod; }