#include "attendance/CorrectionRecord.h"

CorrectionRecord::CorrectionRecord(std::string sID, std::string lecturerID, std::string reasonStr, std::string time)
    : studentID(std::move(sID)), actingLecturerID(std::move(lecturerID)), 
      reason(std::move(reasonStr)), timestamp(std::move(time)) {}

std::string CorrectionRecord::getStudentID() const { return studentID; }
std::string CorrectionRecord::getActingLecturerID() const { return actingLecturerID; }
std::string CorrectionRecord::getReason() const { return reason; }
std::string CorrectionRecord::getTimestamp() const { return timestamp; }