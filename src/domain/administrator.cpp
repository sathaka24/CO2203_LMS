#include "domain/Administrator.h"
#include "domain/Student.h"
#include "domain/Lecturer.h"
#include "domain/LectureCourse.h"
#include "domain/LabCourse.h"
#include "domain/ProjectCourse.h"
#include <iostream>
#include <iomanip>

Administrator::Administrator(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)) {}

Administrator::~Administrator() = default;

void Administrator::showMenu() {
    std::cout << "\n========================================\n";
    std::cout << "        ADMINISTRATOR DASHBOARD         \n";
    std::cout << " Welcome, " << getName() << " (" << getUserID() << ")\n";
    std::cout << "========================================\n";
    std::cout << " 1. Create User Account\n";
    std::cout << " 2. Update User Account\n";
    std::cout << " 3. Remove User Account\n";
    std::cout << " 4. Create Course Offering\n";
    std::cout << " 5. Edit Course Details\n";
    std::cout << " 6. Remove Course Offering\n";
    std::cout << " 7. Generate Enrolment Summary Report\n";
    std::cout << " 8. Generate Attendance Eligibility Report\n";
    std::cout << " 0. Logout\n";
    std::cout << "========================================\n";
    std::cout << "Select option: ";
}

Person* Administrator::createUser(std::string id, std::string name, std::string pass, std::string role) {
    if (role == "Student" || role == "student") {
        return new Student(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Lecturer" || role == "lecturer") {
        return new Lecturer(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Administrator" || role == "Admin" || role == "admin") {
        return new Administrator(std::move(id), std::move(name), std::move(pass));
    }
    std::cerr << "[Error] Unknown user role: " << role << "\n";
    return nullptr;
}

void Administrator::updateUser(std::string id, std::string name, std::string pass) {
    std::cout << "[Admin] User " << id << " updated successfully.\n";
}

void Administrator::removeUser(std::string id) {
    std::cout << "[Admin] User " << id << " marked for deletion.\n";
}

Course* Administrator::createCourse(std::string code, std::string title, int cred, int cap, std::string type) {
    if (type == "Lecture" || type == "lecture") {
        return new LectureCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Lab" || type == "lab") {
        return new LabCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Project" || type == "project") {
        return new ProjectCourse(std::move(code), std::move(title), cred, cap);
    }
    std::cerr << "[Error] Unknown course type: " << type << "\n";
    return nullptr;
}

void Administrator::editCourse(std::string code, std::string title, int cred, int cap) {
    std::cout << "[Admin] Course " << code << " updated successfully.\n";
}

void Administrator::removeCourse(std::string code) {
    std::cout << "[Admin] Course " << code << " marked for removal.\n";
}

void Administrator::generateEnrolmentReport() const {
    std::cout << "\n=======================================================\n";
    std::cout << "           COURSE ENROLMENT SUMMARY REPORT            \n";
    std::cout << "=======================================================\n";
}

void Administrator::generateEligibilityReport(float threshold) const {
    std::cout << "\n=======================================================\n";
    std::cout << "         EXAM ATTENDANCE ELIGIBILITY REPORT            \n";
    std::cout << " Threshold: " << std::fixed << std::setprecision(1) << threshold << "%\n";
    std::cout << "=======================================================\n";
}