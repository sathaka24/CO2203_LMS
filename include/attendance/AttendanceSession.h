#ifndef ATTENDANCE_SESSION_H
#define ATTENDANCE_SESSION_H

#include <string>
#include <vector>
#include "scheduling/TimeSlot.h"
#include "attendance/AttendanceRecord.h"
#include "attendance/CorrectionRecord.h"

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

public:
    AttendanceSession(int id, TimeSlot slot, Course* c);
    ~AttendanceSession();

    void openSession();
    void closeSession();

    void setCaptureMechanism(AttendanceCapture* c); 
    void runCapture();

    void markAttendance(std::string studentID, std::string method);
    void addCorrection(std::string studentID, std::string actingLecturerID, std::string reason);

    bool isSessionOpen() const;
    int getSessionID() const;
    TimeSlot getTimeSlot() const;
    std::vector<AttendanceRecord> getRecords() const;
    std::vector<CorrectionRecord> getCorrections() const;
};

#endif
