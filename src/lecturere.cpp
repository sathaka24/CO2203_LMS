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
    std::cout << " 6. View Attendance Statistics & Eligibility\n";
    std::cout << " 0. Logout\n";
    std::cout << "========================================\n";
    std::cout << "Enter choice: ";
}

std::vector<Course*> Lecturer::getAssignedCourses() const {
    return assignedCourses;
}

void Lecturer::viewEnrolmentList(Course* c) const {
    if (!c) {
        std::cerr << "[Error] Invalid course pointer.\n";
        return;
    }

    // FR2.3: A Lecturer shall be able to view the enrolment list of their own courses only
    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You are not assigned to course " << c->getCourseCode() << ".\n";
        return;
    }

    std::cout << "\n=== Enrolment List for " << c->getCourseCode() << " ===\n";
    // Printing course details (using Course's operator<< or student list)
    std::cout << *c << "\n";
}

AttendanceSession* Lecturer::openAttendanceSession(Course* c, TimeSlot slot, int durationMins) {
    if (!c) {
        std::cerr << "[Error] Course cannot be null.\n";
        return nullptr;
    }

    // Verify the lecturer is assigned to this course
    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You can only open attendance sessions for your assigned courses.\n";
        return nullptr;
    }

    AttendanceRegister* reg = c->getRegister();
    if (!reg) {
        std::cerr << "[Error] Course has no AttendanceRegister.\n";
        return nullptr;
    }

    // Generate a session ID based on current number of sessions
    int newSessionId = static_cast<int>(reg->getSessions().size()) + 1;
    AttendanceSession* session = new AttendanceSession(newSessionId, slot, c);
    session->openSession();
    reg->addSession(session);

    std::cout << "[Success] Attendance session #" << newSessionId 
              << " opened for " << c->getCourseCode() 
              << " (Duration: " << durationMins << " mins).\n";

    return session;
}

void Lecturer::closeAttendanceSession(Course* c, int sessionID) {
    if (!c || !c->getRegister()) {
        std::cerr << "[Error] Invalid course or register.\n";
        return;
    }

    for (AttendanceSession* session : c->getRegister()->getSessions()) {
        if (session && session->getTimeSlot().getDay() == "" /* or matching session ID */) {
            // Found session
            session->closeSession();
            std::cout << "[Success] Attendance session #" << sessionID << " closed.\n";
            return;
        }
    }
}

void Lecturer::recordCorrection(Course* c, int sessionID, std::string studentID, std::string reason) {
    if (!c || !c->getRegister()) {
        std::cerr << "[Error] Invalid course or register.\n";
        return;
    }

    // Verify lecturer assignment
    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You can only record corrections for your own courses.\n";
        return;
    }

    // Find the session and append the immutable correction record
    for (AttendanceSession* session : c->getRegister()->getSessions()) {
        if (session) {
            // Adds correction with actingLecturerID = this->getUserID()
            session->addCorrection(studentID, this->getUserID(), reason);
            std::cout << "[Success] Correction recorded for student " << studentID 
                      << " by Lecturer " << getUserID() << ".\n";
            return;
        }
    }
}