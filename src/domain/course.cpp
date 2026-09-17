#include "domain/Course.h"
#include "domain/Lecturer.h"
#include "scheduling/Timetable.h"
#include "attendance/AttendanceRegister.h"
#include <iostream>
#include <algorithm>

Course::Course(std::string code, std::string title, int cred, int cap)
    : courseCode(std::move(code)),
      title(std::move(title)),
      credits(cred),
      capacity(cap),
      assignedLecturer(nullptr),
      courseTimetable(new Timetable()),             // Composition: Course owns its Timetable
      attendanceRegister(new AttendanceRegister()) { // Composition: Course owns its Register
}

Course::~Course() {
    delete courseTimetable;
    courseTimetable = nullptr;

    delete attendanceRegister;
    attendanceRegister = nullptr;
}

std::string Course::getCourseCode() const {
    return courseCode;
}

int Course::getCapacity() const {
    return capacity;
}

Timetable* Course::getTimetable() const {
    return courseTimetable;
}

AttendanceRegister* Course::getRegister() const {
    return attendanceRegister;
}

std::vector<Course*> Course::getPrerequisites() const {
    return prerequisites;
}

void Course::assignLecturer(Lecturer* l) {
    assignedLecturer = l;
}

void Course::addPrerequisite(Course* c) {
    if (c && c != this) {
        auto it = std::find(prerequisites.begin(), prerequisites.end(), c);
        if (it == prerequisites.end()) {
            prerequisites.push_back(c);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Course& c) {
    os << "[" << c.courseCode << "] " << c.title 
       << " | Type: " << c.getCourseType()
       << " | Credits: " << c.credits 
       << " | Capacity: " << c.capacity;
    if (c.assignedLecturer) {
        os << " | Lecturer: " << c.assignedLecturer->getName();
    } else {
        os << " | Lecturer: Unassigned";
    }
    return os;
}

std::string Course::getTitle() const {
    return title;
}

int Course::getCredits() const {
    return credits;
}

Lecturer* Course::getLecturer() const {
    return assignedLecturer;
}

int Course::getEnrolledCount() const {
    return static_cast<int>(enrolledStudents.size());
}

const std::vector<Student*>& Course::getEnrolledStudents() const {
    return enrolledStudents;
}

void Course::addStudent(Student* s) {
    if (!s) {
        return;
    }
    auto it = std::find(enrolledStudents.begin(), enrolledStudents.end(), s);
    if (it == enrolledStudents.end()) {
        enrolledStudents.push_back(s);
    }
}

void Course::removeStudent(Student* s) {
    
    auto it = std::find(enrolledStudents.begin(), enrolledStudents.end(), s);
    if (it != enrolledStudents.end()) {
        enrolledStudents.erase(it);
    }
}

void Course::setTitle(const std::string& newTitle) { 
    title = newTitle; 
}
void Course::setCredits(int newCredits) {
    credits = newCredits; 

}
void Course::setCapacity(int newCapacity) {
    capacity = newCapacity;

}

void Course::removePrerequisite(Course* c) {

    auto it = std::find(prerequisites.begin(), prerequisites.end(), c);
    
    if (it != prerequisites.end()) {
        prerequisites.erase(it);
    }
}