#ifndef LECTURER_H
#define LECTURER_H

#include <vector>
#include <string>
#include "domain/Person.h"
#include "scheduling/TimeSlot.h"

class Course;              // forward declaration - M1
class AttendanceSession;   // forward declaration - M2

// Owner: M1
// Used by: M2 (Course::assignedLecturer), M3 (UserRepository).

class Lecturer : public Person {
private:
    std::vector<Course*> assignedCourses;   // aggregation

public:
    Lecturer(std::string id, std::string name, std::string pass);
    ~Lecturer() override;

    void showMenu() override;

    std::vector<Course*> getAssignedCourses() const;
    void viewEnrolmentList(Course* c) const;

    AttendanceSession* openAttendanceSession(Course* c, TimeSlot slot, int durationMins);
    void closeAttendanceSession(Course* c, int sessionID);
    void recordCorrection(Course* c, int sessionID, std::string studentID, std::string reason);
};

#endif
