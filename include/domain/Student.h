#ifndef STUDENT_H
#define STUDENT_H

#include <vector>
#include "domain/Person.h"

class Course;      // forward declaration - M1
class Timetable;   // forward declaration - M2

// Owner: M1
// Used by: M2 (EnrollmentEngine), M3 (UserRepository).


class Student : public Person {
private:
    std::vector<Course*> enrolledCourses;   // aggregation: does NOT own courses
    Timetable* personalTimetable;           // composition: owns this

public:
    Student(std::string id, std::string name, std::string pass);
    ~Student() override;

    void showMenu() override;

    Timetable* getTimetable() const;
    void addCourse(Course* c);
    void removeCourse(Course* c);
    const std::vector<Course*>& getEnrolledCourses() const;
};

#endif
