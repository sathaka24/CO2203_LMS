#ifndef STUDENT_H
#define STUDENT_H

#include <vector>
#include <string>
#include "domain/Person.h"

class Course;      // forward declaration - M1
class Timetable;   // forward declaration - M2

// Owner: M1
// Used by: M2 (EnrollmentEngine), M3 (UserRepository).


class Student : public Person {
private:
    std::vector<Course*> enrolledCourses;   // aggregation: does NOT own courses
    Timetable* personalTimetable;           // composition: owns this

    std::vector<std::string> completedCourses;  // course codes already passed

public:
    Student(std::string id, std::string name, std::string pass);
    ~Student() override;

    void showMenu() override;

    Timetable* getTimetable() const;
    void addCourse(Course* c);
    void removeCourse(Course* c);
    const std::vector<Course*>& getEnrolledCourses() const;
    bool hasCompletedCourse(const std::string& courseCode) const;
    void addCompletedCourse(const std::string& courseCode);
    const std::vector<std::string>& getCompletedCourses() const;
};

#endif
