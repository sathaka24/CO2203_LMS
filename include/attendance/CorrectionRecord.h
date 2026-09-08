#ifndef CORRECTION_RECORD_H
#define CORRECTION_RECORD_H

#include <string>

// Owner: M2
// Used by: M3 (CourseRepository save/load).

class CorrectionRecord {
private:
    std::string studentID;
    std::string actingLecturerID;
    std::string reason;
    std::string timestamp;

public:
    CorrectionRecord(std::string sID, std::string lecturerID, std::string reason, std::string time);

    std::string getStudentID() const;
    std::string getActingLecturerID() const;
    std::string getReason() const;
    std::string getTimestamp() const;
};

#endif
