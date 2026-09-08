#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

class Lecturer;             // forward declaration - M1
class Timetable;            // forward declaration - M2
class AttendanceRegister;   // forward declaration - M2

// Owner: M1
// Used by: M2 (EnrollmentEngine, AttendanceSession), M3 (CourseRepository).

class Course {
protected:
    std::string courseCode;
    std::string title;
    int credits;
    int capacity;
    Lecturer* assignedLecturer;              // aggregation
    std::vector<Course*> prerequisites;      // aggregation
    Timetable* courseTimetable;              // composition - owned
    AttendanceRegister* attendanceRegister;  // composition - owned

public:
    Course(std::string code, std::string title, int cred, int cap);
    virtual ~Course();

    std::string getCourseCode() const;
    int getCapacity() const;
    Timetable* getTimetable() const;
    AttendanceRegister* getRegister() const;
    std::vector<Course*> getPrerequisites() const;

    void assignLecturer(Lecturer* l);
    void addPrerequisite(Course* c);

    virtual std::string getCourseType() const = 0;
    virtual float calculateFinalGrade(std::map<std::string, float> componentScores) const = 0;
    virtual std::map<std::string, float> getAssessmentBreakdown() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Course& c);
};

#endif
