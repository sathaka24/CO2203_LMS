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

    // Validation helpers (Must have 'const' to match .cpp)
    void checkPrerequisites(Student* s, Course* c) const;   
    void checkClashes(Student* s, Course* c) const;         

public:
    EnrollmentEngine(UserRepository* u, CourseRepository* c);
    ~EnrollmentEngine() = default;

    // Must use const std::string& to match .cpp
    void enrolStudent(const std::string& sID, const std::string& cCode);   
    void dropStudent(const std::string& sID, const std::string& cCode);    
};

#endif // ENROLLMENT_ENGINE_H