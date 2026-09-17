#include "scheduling/EnrollmentEngine.h"
#include "domain/Student.h"
#include "domain/Course.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "scheduling/Timetable.h"
#include "exception/Exceptions.h"
#include <stdexcept>
#include <iostream>

EnrollmentEngine::EnrollmentEngine(UserRepository* uRepo, CourseRepository* cRepo)
    : userRepo(uRepo), courseRepo(cRepo) {}

void EnrollmentEngine::checkPrerequisites(Student* student, Course* course) const {
    // Assuming Course has a getPrerequisites() returning a vector of Course pointers
    // and Student has a method to check if they have completed/enrolled in it.
    // (This logic might need adjustment based on exactly how Member 1 implements Student).
    auto prerequisites = course->getPrerequisites();
    
    for (Course* prereq : prerequisites) {
        if (!student->hasCompletedCourse(prereq->getCourseCode())) {
            throw PrerequisiteNotMetException("Missing prerequisite " + prereq->getCourseCode());
        }
    }
}

void EnrollmentEngine::checkClashes(Student* student, Course* course) const {
    Timetable* studentTimetable = student->getTimetable();
    Timetable* courseTimetable = course->getTimetable();

    if (!studentTimetable || !courseTimetable) return;

    // Iterate through course slots and check against student's timetable
    for (TimeSlot* slot : courseTimetable->getSlots()) {
        if (studentTimetable->checkClash(*slot)) {
            throw TimetableClashException("Time slot clash detected");
        }
    }
}

void EnrollmentEngine::enrolStudent(const std::string& studentID, const std::string& courseCode) {
    // 1. Fetch Student and Course from Repositories (Owned by M3)
    Person* p = userRepo->get(studentID);
    Student* student = dynamic_cast<Student*>(p);
    Course* course = courseRepo->get(courseCode);

    if (!student) throw EnrollmentException("Student " + studentID + " not found");
    if (!course)  throw EnrollmentException("Course " + courseCode + " not found");

    // 2. Check if course is full
    if (course->getEnrolledCount() >= course->getCapacity()) {
        throw CourseFullException("Course " + courseCode + " is at maximum capacity");
    }

    // 3. Validate rules (Owned by M2)
    checkPrerequisites(student, course);
    checkClashes(student, course);

    // 4. Perform Enrollment (Owned by M1)
    student->addCourse(course);
    course->addStudent(student);

    std::cout << "[Success] Student " << studentID << " enrolled in " << courseCode << "\n";
}

void EnrollmentEngine::dropStudent(const std::string& studentID, const std::string& courseCode) {
    Person* p = userRepo->get(studentID);
    Student* student = dynamic_cast<Student*>(p);
    Course* course = courseRepo->get(courseCode);

    if (!student || !course) {
        throw EnrollmentException("Student or course not found");
    }

    student->removeCourse(course);
    course->removeStudent(student);
    
    std::cout << "[Success] Student " << studentID << " dropped " << courseCode << "\n";
}