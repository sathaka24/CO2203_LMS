#include "attendance/AttendanceSession.h"
#include "attendance/AttendanceCapture.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

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
        throw std::runtime_error("SessionClosedException: Session #" + std::to_string(sessionID) + " is closed.");
    }

    // Check duplicate
    for (const auto& rec : records) {
        if (rec.getStudentID() == studentID) {
            throw std::runtime_error("DuplicateAttendanceException: Student " + studentID + " already recorded.");
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
        std::string studentID = capture->captureNext();
        if (studentID.empty()) {
            break;
        }
        if (studentID == "INVALID_TOKEN") {
            continue;
        }

        try {
            markAttendance(studentID, "QR_CAPTURE");
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