#ifndef ATTENDANCE_REGISTER_H
#define ATTENDANCE_REGISTER_H

#include <string>
#include <vector>

class AttendanceSession;   // forward declaration

// Owner: M2
// Used by: M1 (Course owns one, Administrator reports), M3 (CourseRepository).
class AttendanceRegister {
private:
    std::vector<AttendanceSession*> sessions;   
    static constexpr float DEFAULT_THRESHOLD = 80.0f;   // static member

public:
    AttendanceRegister();
    ~AttendanceRegister();

    
    AttendanceRegister(const AttendanceRegister&) = delete;
    AttendanceRegister& operator=(const AttendanceRegister&) = delete;

    void addSession(AttendanceSession* s);
    std::vector<AttendanceSession*> getSessions() const;

    float calculateStudentPercentage(std::string studID) const;   
    float calculateCoursePercentage() const;                      
    std::vector<std::string> getEligibilityReport(float threshold = DEFAULT_THRESHOLD) const;
};

#endif 
