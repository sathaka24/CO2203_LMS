#ifndef ENROLLMENT_ENGINE_H
#define ENROLLMENT_ENGINE_H

#include <string>

class UserRepository;     // forward declaration - M3
class CourseRepository;   // forward declaration - M3
class Student;            // forward declaration - M1
class Course;             // forward declaration - M1

// Owner: M2  (see change log entry CL-001 - ownership decision)
// Used by: M1 (Student::showMenu), M3 (main.cpp).


class EnrollmentEngine {
private:
    UserRepository* userRepo;       // aggregation
    CourseRepository* courseRepo;   // aggregation

    void checkPrerequisites(Student* s, Course* c);   
    void checkClashes(Student* s, Course* c);         
    void checkCapacity(Course* c);                    

public:
    EnrollmentEngine(UserRepository* u, CourseRepository* c);
    ~EnrollmentEngine();

    void enrolStudent(std::string sID, std::string cCode);   
    void dropStudent(std::string sID, std::string cCode);    
};

#endif
