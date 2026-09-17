#ifndef ATTENDANCE_SESSION_H
#define ATTENDANCE_SESSION_H

#include <string>
#include <vector>
#include "scheduling/TimeSlot.h"
#include "attendance/AttendanceRecord.h"
#include "attendance/CorrectionRecord.h"
#include <ctime>

class Course;              
class AttendanceCapture;   

// Owner: M2
// Used by: M1 (Lecturer, Lecturer::showMenu), M3 (CourseRepository).
// This file and AttendanceSession.cpp must not include QRCodeCapture.h or
// FileReplayCapture.h

class AttendanceSession {
private:
    int sessionID;
    TimeSlot timeSlot;
    bool isOpen;
    std::vector<AttendanceRecord> records;
    std::vector<CorrectionRecord> corrections;
    Course* course;                 // aggregation - back-reference
    AttendanceCapture* capture;     // aggregation - runtime swappable, not owned

    std::time_t openedAt;   // when the session was opened
    int durationMins;       // 0 = no automatic expiry

public:
    AttendanceSession(int id, TimeSlot slot, Course* c);
    ~AttendanceSession();

    void openSession(int durationMins = 0);
    void closeSession();

    void restoreOpenState(std::time_t openedAt, int durationMins);
    bool isExpired() const;
    std::time_t getOpenedAt() const;
    int getDurationMins() const;

    void setCaptureMechanism(AttendanceCapture* c); 
    void runCapture();

    void markAttendance(std::string studentID, std::string method);
    void addCorrection(std::string studentID, std::string actingLecturerID, std::string reason, std::string status);

    void restoreRecord(const AttendanceRecord& r);
    void restoreCorrection(const CorrectionRecord& c);

    bool isSessionOpen() const;
    int getSessionID() const;
    TimeSlot getTimeSlot() const;
    std::vector<AttendanceRecord> getRecords() const;
    std::vector<CorrectionRecord> getCorrections() const;
};

#endif
