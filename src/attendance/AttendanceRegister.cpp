#include "attendance/AttendanceRegister.h"
#include "attendance/AttendanceSession.h"
#include <iostream>

AttendanceRegister::AttendanceRegister() = default;

AttendanceRegister::~AttendanceRegister() {
    for (auto* s : sessions) {
        delete s;
    }
    sessions.clear();
}

void AttendanceRegister::addSession(AttendanceSession* s) {
    if (s != nullptr) {
        sessions.push_back(s);
    }
}

std::vector<AttendanceSession*> AttendanceRegister::getSessions() const {
    return sessions;
}

float AttendanceRegister::calculateStudentPercentage(std::string studID) const {
    int totalHeld = 0;
    int attended = 0;

    for (const auto* s : sessions) {
        if (!s->isSessionOpen()) { // Count only concluded sessions
            totalHeld++;
            for (const auto& rec : s->getRecords()) {
                if (rec.getStudentID() == studID && rec.getStatus() == "PRESENT") {
                    attended++;
                    break;
                }
            }
        }
    }

    if (totalHeld == 0) return 100.0f;
    return (static_cast<float>(attended) / static_cast<float>(totalHeld)) * 100.0f;
}

float AttendanceRegister::calculateCoursePercentage() const {
    if (sessions.empty()) return 100.0f;
    
    int totalSessionRecords = 0;
    for (const auto* s : sessions) {
        totalSessionRecords += static_cast<int>(s->getRecords().size());
    }
    return (sessions.empty()) ? 0.0f : static_cast<float>(totalSessionRecords) / static_cast<float>(sessions.size());
}

std::vector<std::string> AttendanceRegister::getEligibilityReport(float threshold) const {
    std::vector<std::string> report;
    std::cout << "\n--- Attendance Eligibility Report (Threshold: " << threshold << "%) ---\n";
    // Returns formatted status strings for reporting
    return report;
}