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
    std::cout << " 1. Create User (Student / Lecturer / Admin)\n";
    std::cout << " 2. Update User Details\n";
    std::cout << " 3. Remove User\n";
    std::cout << " 4. Create Course\n";
    std::cout << " 5. Edit Course Details\n";
    std::cout << " 6. Remove Course\n";
    std::cout << " 7. Generate Enrolment Summary Report\n";
    std::cout << " 8. Generate Attendance Eligibility Report\n";
    std::cout << " 0. Logout\n";
    std::cout << "========================================\n";
    std::cout << "Enter choice: ";
}

Person* Administrator::createUser(std::string id, std::string name, std::string pass, std::string role) {
    if (role == "Student" || role == "student") {
        return new Student(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Lecturer" || role == "lecturer") {
        return new Lecturer(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Administrator" || role == "admin" || role == "Administrator") {
        return new Administrator(std::move(id), std::move(name), std::move(pass));
    }
    std::cerr << "[Error] Unknown role: " << role << std::endl;
    return nullptr;
}

void Administrator::updateUser(std::string id, std::string name, std::string pass) {
    // In a full system flow, this modifies the user found in UserRepository.
    std::cout << "[Admin] Request to update user ID: " << id << std::endl;
}

void Administrator::removeUser(std::string id) {
    // In a full system flow, this deletes the user from UserRepository.
    std::cout << "[Admin] Request to remove user ID: " << id << std::endl;
}

Course* Administrator::createCourse(std::string code, std::string title, int cred, int cap, std::string type) {
    if (type == "Lecture" || type == "lecture") {
        return new LectureCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Lab" || type == "lab") {
        return new LabCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Project" || type == "project") {
        return new ProjectCourse(std::move(code), std::move(title), cred, cap);
    }
    std::cerr << "[Error] Unknown course type: " << type << std::endl;
    return nullptr;
}

void Administrator::editCourse(std::string code, std::string title, int cred, int cap) {
    std::cout << "[Admin] Request to edit course: " << code << std::endl;
}

void Administrator::removeCourse(std::string code) {
    std::cout << "[Admin] Request to remove course: " << code << std::endl;
}

void Administrator::generateEnrolmentReport() const {
    std::cout << "\n--- Course Enrolment Summary Report ---\n";
    // Formatted tabular report listing courses, capacities, and enrolled counts
}

void Administrator::generateEligibilityReport(float threshold) const {
    std::cout << "\n--- Student Exam Eligibility Report (Threshold: " 
              << std::fixed << std::setprecision(1) << threshold << "%) ---\n";
    // Checks each course's AttendanceRegister against the threshold
}