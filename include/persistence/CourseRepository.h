#ifndef COURSE_REPOSITORY_H
#define COURSE_REPOSITORY_H

#include <string>
#include "persistence/Repository.h"
#include "persistence/UserRepository.h"
#include "domain/Course.h"

// Owner: M3
// Used by: M1 (Administrator::showMenu), M2 (EnrollmentEngine), M3 (main.cpp).

class CourseRepository : public Repository<Course> {

private:
    UserRepository* userRepo;
    std::string attendanceFile;

    void saveAttendance() const;
    void loadAttendance();

public:
    explicit CourseRepository(UserRepository* users, std::string attendanceFile = "data/attendance.txt");

    void save(const std::string& filename) override;   
    void load(const std::string& filename) override;  
};

#endif
