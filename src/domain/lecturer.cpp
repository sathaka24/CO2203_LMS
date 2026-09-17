#include "domain/Lecturer.h"
#include "domain/Course.h"
#include "attendance/AttendanceSession.h"
#include "attendance/AttendanceRegister.h"
#include "scheduling/TimeSlot.h"
#include <iostream>
#include <algorithm>

Lecturer::Lecturer(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)) {}

Lecturer::~Lecturer() = default;

void Lecturer::showMenu() {
    std::cout << "\n========================================\n";
    std::cout << "           LECTURER DASHBOARD           \n";
    std::cout << " Welcome, " << getName() << " (" << getUserID() << ")\n";
    std::cout << "========================================\n";
    std::cout << " 1. View Assigned Courses\n";
    std::cout << " 2. View Course Enrolment List\n";
    std::cout << " 3. Open Attendance Session\n";
    std::cout << " 4. Close Attendance Session\n";
    std::cout << " 5. Record Attendance Correction\n";
    std::cout << " 6. View Attendance Statistics\n";
    std::cout << " 0. Logout\n";
    std::cout << "========================================\n";
    std::cout << "Select option: ";
}

std::vector<Course*> Lecturer::getAssignedCourses() const {
    return assignedCourses;
}

void Lecturer::viewEnrolmentList(Course* c) const {
    if (!c) {
        std::cerr << "[Error] Course pointer is null.\n";
        return;
    }

    // FR2.3: Only view enrolment for their own assigned courses
    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You are not the assigned lecturer for course " 
                  << c->getCourseCode() << ".\n";
        return;
    }

    std::cout << "\n--- Enrolment List for Course: " << c->getCourseCode() << " ---\n";
    std::cout << *c << "\n";
}

AttendanceSession* Lecturer::openAttendanceSession(Course* c, TimeSlot slot, int durationMins) {
    if (!c) {
        std::cerr << "[Error] Course cannot be null.\n";
        return nullptr;
    }

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You cannot open sessions for an unassigned course.\n";
        return nullptr;
    }

    AttendanceRegister* reg = c->getRegister();
    if (!reg) {
        std::cerr << "[Error] Course has no AttendanceRegister.\n";
        return nullptr;
    }

    int nextSessionID = static_cast<int>(reg->getSessions().size()) + 1;
    AttendanceSession* session = new AttendanceSession(nextSessionID, slot, c);
    session->openSession();
    reg->addSession(session);

    std::cout << "[Success] Opened Attendance Session #" << nextSessionID 
              << " for " << c->getCourseCode() 
              << " (Active duration: " << durationMins << " mins).\n";

    return session;
}

void Lecturer::closeAttendanceSession(Course* c, int sessionID) {
    if (!c || !c->getRegister()) {
        std::cerr << "[Error] Invalid course or register.\n";
        return;
    }

    for (AttendanceSession* session : c->getRegister()->getSessions()) {
        if (session) {
            session->closeSession();
            std::cout << "[Success] Attendance Session #" << sessionID << " closed.\n";
            return;
        }
    }
}

void Lecturer::recordCorrection(Course* c, int sessionID, std::string studentID, std::string reason) {
    if (!c || !c->getRegister()) {
        std::cerr << "[Error] Invalid course or register.\n";
        return;
    }

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You can only correct attendance for your own courses.\n";
        return;
    }

    for (AttendanceSession* session : c->getRegister()->getSessions()) {
        if (session) {
            session->addCorrection(studentID, this->getUserID(), reason);
            std::cout << "[Success] Correction appended for Student: " << studentID 
                      << " (Lecturer: " << getUserID() << ").\n";
            return;
        }
    }
}

void Lecturer::removeAssignedCourse(Course* c) {

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    
    if (it != assignedCourses.end()) {
        assignedCourses.erase(it);
    }
}