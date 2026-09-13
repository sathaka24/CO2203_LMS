#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

class BaseException : public exception {
protected:
    string message;

public:
    explicit BaseException(string msg);
    virtual ~BaseException();

    // Returns the message.
    const char* what() const noexcept override;
};

// ---------------------------------------------------------------------------
// Enrolment
// ---------------------------------------------------------------------------

class EnrollmentException : public BaseException {
public:
    explicit EnrollmentException(string msg);
};

// The course has already reached its capacity.
class CourseFullException : public EnrollmentException {
public:
    explicit CourseFullException(string msg);
};

// The student has not completed a required prerequisite course.
class PrerequisiteNotMetException : public EnrollmentException {
public:
    explicit PrerequisiteNotMetException(string msg);
};

// The new course overlaps a time slot already in the student's timetable.
class TimetableClashException : public EnrollmentException {
public:
    explicit TimetableClashException(string msg);
};

// ---------------------------------------------------------------------------
// Attendance
// ---------------------------------------------------------------------------

class AttendanceException : public BaseException {
public:
    explicit AttendanceException(string msg);
};

// Attendance was marked on a session that is closed (or not open yet).
class SessionClosedException : public AttendanceException {
public:
    explicit SessionClosedException(string msg);
};

// The student is not enrolled in the course the session belongs to.
class NotEnrolledException : public AttendanceException {
public:
    explicit NotEnrolledException(string msg);
};

// The student has already been marked present in this session.
class DuplicateAttendanceException : public AttendanceException {
public:
    explicit DuplicateAttendanceException(string msg);
};

// ---------------------------------------------------------------------------
// Storage
// ---------------------------------------------------------------------------

class StorageException : public BaseException {
public:
    explicit StorageException(string msg);
};

// A line in a data file has the wrong number of fields, a bad number,
// a duplicate ID, or refers to something that does not exist.
class DataCorruptedException : public StorageException {
public:
    explicit DataCorruptedException(string msg);
};

// The data file could not be opened for reading or writing.
class MissingFileException : public StorageException {
public:
    explicit MissingFileException(string msg);
};

#endif // EXCEPTIONS_H
