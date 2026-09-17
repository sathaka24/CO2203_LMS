#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <string>

class UserRepository;     // M3
class CourseRepository;   // M3
class EnrollmentEngine;   // M2

// Created once in main() and passed to showMenu() of whoever logs in.
// It only holds references, so it owns nothing.
struct SystemContext {
    UserRepository&   users;
    CourseRepository& courses;
    EnrollmentEngine& enrollment;
    std::string       usersFile;
    std::string       coursesFile;
};

#endif