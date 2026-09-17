#include "attendance/AttendanceSession.h"
#include "attendance/AttendanceCapture.h"
#include "domain/Course.h"
#include "domain/Student.h"
#include "exception/Exceptions.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

static std::string generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeNow), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

AttendanceSession::AttendanceSession(int id, TimeSlot slot, Course* c)
    : sessionID(id), timeSlot(slot), isOpen(false), course(c), capture(nullptr) {}

AttendanceSession::~AttendanceSession() = default;

void AttendanceSession::openSession() {
    isOpen = true;
}

void AttendanceSession::closeSession() {
    isOpen = false;
}

void AttendanceSession::setCaptureMechanism(AttendanceCapture* c) {
    capture = c;
}

void AttendanceSession::markAttendance(std::string studentID, std::string method) {
    if (!isOpen) {

        throw SessionClosedException("Session #" + std::to_string(sessionID) + " is closed.");
    }
    if (course == nullptr) {

        throw AttendanceException("Session #" + std::to_string(sessionID) + " is not linked to a course.");
    }

    bool enrolled = false;
    for (const Student* s : course->getEnrolledStudents()) {


        if (s != nullptr && s->getUserID() == studentID) {

            enrolled = true;
            break;
        }
    }
    if (!enrolled) {
        throw NotEnrolledException("Student " + studentID + " is not enrolled in " + course->getCourseCode() + ".");
    }

    for (const auto& rec : records) {

        if (rec.getStudentID() == studentID) {

            throw DuplicateAttendanceException("Student " + studentID + " already recorded.");
        }
    }

    records.emplace_back(studentID, generateTimestamp(), "PRESENT", method);
}

void AttendanceSession::addCorrection(std::string studentID, std::string actingLecturerID, std::string reason) {
    corrections.emplace_back(studentID, actingLecturerID, reason, generateTimestamp());
}

void AttendanceSession::runCapture() {
    if (!capture) {
        std::cout << " [ERROR] No capture mechanism assigned to session #" << sessionID << "\n";
        return;
    }

    if (!isOpen) {
        openSession();
    }

    capture->beginSession();

    while (true) {
        
        std::string studentID;
        try {
            studentID = capture->captureNext();
        } catch (const std::exception& e) {
            // a malformed event is reported and skipped, the capture continues
            std::cout << " -> Rejected: " << e.what() << "\n";
            continue;
        }

        if (studentID.empty()) {
            break;
        }
        if (studentID == "INVALID_TOKEN") {
            continue;
        }

        try {

            markAttendance(studentID, capture->getMethodName());

            std::cout << " -> Success: Attendance recorded for " << studentID << "\n";
        } catch (const std::exception& e) {

            std::cout << " -> Failed: " << e.what() << "\n";
        }
    }

    capture->endSession();
}

bool AttendanceSession::isSessionOpen() const { return isOpen; }
int AttendanceSession::getSessionID() const { return sessionID; }
TimeSlot AttendanceSession::getTimeSlot() const { return timeSlot; }
std::vector<AttendanceRecord> AttendanceSession::getRecords() const { return records; }
std::vector<CorrectionRecord> AttendanceSession::getCorrections() const { return corrections; }

void AttendanceSession::restoreRecord(const AttendanceRecord& r) {
    records.push_back(r);
}

void AttendanceSession::restoreCorrection(const CorrectionRecord& c) {
    corrections.push_back(c);
}