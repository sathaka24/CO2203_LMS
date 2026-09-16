#include "domain/Student.h"
#include "domain/Course.h"
#include "scheduling/Timetable.h"
#include <iostream>
#include <algorithm>

Student::Student(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)),
      personalTimetable(new Timetable()) { // Composition: Student strictly owns its Timetable
}

Student::~Student() {
    delete personalTimetable;
    personalTimetable = nullptr;
}

void Student::showMenu() {
    std::cout << "\n========================================\n";
    std::cout << "           STUDENT DASHBOARD            \n";
    std::cout << " Welcome, " << getName() << " (" << getUserID() << ")\n";
    std::cout << "========================================\n";
    std::cout << " 1. View Enrolled Courses\n";
    std::cout << " 2. View Weekly Timetable\n";
    std::cout << " 3. Enrol in a Course\n";
    std::cout << " 4. Drop a Course\n";
    std::cout << " 5. Check-In to Attendance Session\n";
    std::cout << " 6. View My Attendance Record\n";
    std::cout << " 0. Logout\n";
    std::cout << "========================================\n";
    std::cout << "Select option: ";
}

Timetable* Student::getTimetable() const {
    return personalTimetable;
}

void Student::addCourse(Course* c) {
    if (!c) return;

    auto it = std::find(enrolledCourses.begin(), enrolledCourses.end(), c);
    if (it == enrolledCourses.end()) {
        enrolledCourses.push_back(c);

        // Copy course slots into personal timetable
        if (personalTimetable && c->getTimetable()) {
            for (TimeSlot* slot : c->getTimetable()->getSlots()) {
                if (slot) {
                    personalTimetable->addSlot(*slot);
                }
            }
        }
    }
}

void Student::removeCourse(Course* c) {
    if (!c) return;

    auto it = std::find(enrolledCourses.begin(), enrolledCourses.end(), c);
    if (it != enrolledCourses.end()) {
        enrolledCourses.erase(it);

        // Rebuild timetable from remaining enrolled courses
        delete personalTimetable;
        personalTimetable = new Timetable();
        for (Course* course : enrolledCourses) {
            if (course && course->getTimetable()) {
                for (TimeSlot* slot : course->getTimetable()->getSlots()) {
                    if (slot) {
                        personalTimetable->addSlot(*slot);
                    }
                }
            }
        }
    }
}

const std::vector<Course*>& Student::getEnrolledCourses() const {
    return enrolledCourses;
}

bool Student::hasCompletedCourse(const std::string& courseCode) const {

    if (std::find(completedCourses.begin(), completedCourses.end(), courseCode) != completedCourses.end())
    {
        return true;
    } else {

        return false;
    }
    
}

void Student::addCompletedCourse(const std::string& courseCode) {
    if (!hasCompletedCourse(courseCode)) {
        completedCourses.push_back(courseCode);
    }
}

const std::vector<std::string>& Student::getCompletedCourses() const {
    return completedCourses;
}