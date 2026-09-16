#include "Exceptions.h"

#include <utility>


BaseException::BaseException(string msg) : message(move(msg)) {}

BaseException::~BaseException() {}

const char* BaseException::what() const noexcept {
    return message.c_str();
}


EnrollmentException::EnrollmentException(string msg)
    : BaseException(move(msg)) {}

CourseFullException::CourseFullException(string msg)
    : EnrollmentException(move(msg)) {}

PrerequisiteNotMetException::PrerequisiteNotMetException(string msg)
    : EnrollmentException(move(msg)) {}

TimetableClashException::TimetableClashException(string msg)
    : EnrollmentException(move(msg)) {}

AttendanceException::AttendanceException(string msg)
    : BaseException(move(msg)) {}

SessionClosedException::SessionClosedException(string msg)
    : AttendanceException(move(msg)) {}

NotEnrolledException::NotEnrolledException(string msg)
    : AttendanceException(move(msg)) {}

DuplicateAttendanceException::DuplicateAttendanceException(string msg)
    : AttendanceException(move(msg)) {}

StorageException::StorageException(string msg)
    : BaseException(move(msg)) {}

DataCorruptedException::DataCorruptedException(string msg)
    : StorageException(move(msg)) {}

MissingFileException::MissingFileException(string msg)
    : StorageException(move(msg)) {}
