#include "domain/Student.h"
#include "domain/Course.h"
#include "scheduling/Timetable.h"
#include "app/SystemContext.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "scheduling/EnrollmentEngine.h"
#include "attendance/AttendanceRegister.h"
#include "attendance/AttendanceSession.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

Student::Student(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)),
      personalTimetable(new Timetable()) { // Composition: Student strictly owns its Timetable
}

Student::~Student() {
    delete personalTimetable;
    personalTimetable = nullptr;
}

void Student::showMenu(SystemContext& ctx) {

    // here we use lambda function get the whole input line (same style as admin/lecturer menu)
    auto readLine = [](const std::string& prompt) {
        std::string s;
        std::cout << prompt;
        if (!std::getline(std::cin, s)) {
            throw std::runtime_error("Input closed");
        }
        return s;
    };

    // here we use lambda function to get integer, asking again on bad input
    auto readInt = [&](const std::string& prompt) {
        while (true) {
            std::string s = readLine(prompt);
            try {
                std::size_t used = 0;
                int value = std::stoi(s, &used);
                if (used == s.size()) {
                    return value;
                }
            } catch (const std::exception&) {}
            std::cout << "Please enter a whole number.\n";
        }
    };

    // this use to pick a course from enrolled
    auto pickFrom = [&](const std::vector<Course*>& list) -> Course* {
        if (list.empty()) {
            std::cout << "No courses to show.\n";
            return nullptr;
        }
        for (std::size_t i = 0; i < list.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << *list[i]
                      << " | Enrolled: " << list[i]->getEnrolledCount()
                      << "/" << list[i]->getCapacity() << "\n";
        }
        int choice = readInt("Select course (0 to cancel): ");
        if (choice < 1 || choice > static_cast<int>(list.size())) {
            return nullptr;
        }
        return list[choice - 1];
    };

    // our main loop
    while (true) {
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

        try {
            int choice = readInt("Select option: ");
            bool changed = false;

            switch (choice) {

            case 0:
                std::cout << "Logging out...\n";
                return;

            // view enrolled courses
            case 1: {
                if (enrolledCourses.empty()) {

                    std::cout << "You are not enrolled in any course.\n";
                }
                for (Course* c : enrolledCourses) {

                    std::cout << "  " << *c << "\n";
                }
                break;
            }

            // view personal timetable
            case 2: {

                if (!personalTimetable || personalTimetable->getSlots().empty()) {
                    std::cout << "Your timetable is empty.\n";
                } else {
                    std::cout << *personalTimetable;
                }
                break;
            }

            // enrol to a course
            case 3: {

                std::vector<Course*> available;
                for (Course* c : ctx.courses.getAll()) {

                    if (std::find(enrolledCourses.begin(), enrolledCourses.end(), c) == enrolledCourses.end()) {
                        available.push_back(c);
                    }
                }

                Course* c = pickFrom(available);
                if (!c) break;

                // engine checks capacity, prerequisites and clashes, and throws on failure
                ctx.enrollment.enrolStudent(getUserID(), c->getCourseCode());
                changed = true;
                break;
            }

            // drop a enrolled course
            case 4: {
                std::vector<Course*> mine = enrolledCourses;

                Course* c = pickFrom(mine);

                if (!c) break;

                std::string confirm = readLine("Drop " + c->getCourseCode() + "? (y/n): ");
                if (confirm != "y" && confirm != "Y") {
                    std::cout << "Cancelled.\n";
                    break;
                }

                ctx.enrollment.dropStudent(getUserID(), c->getCourseCode());
                changed = true;
                break;
            }

            // check-in to an open session of an enrolled course
            case 5: {

                std::vector<std::pair<Course*, AttendanceSession*>> open;

                for (Course* c : enrolledCourses) {

                    for (AttendanceSession* s : c->getRegister()->getSessions()) {

                        if (s->isSessionOpen()) {
                            open.push_back({c, s});
                        }
                    }
                }

                if (open.empty()) {
                    std::cout << "There are no open sessions for your courses right now.\n";
                    break;
                }

                for (std::size_t i = 0; i < open.size(); ++i) {

                    std::cout << "  " << (i + 1) << ". " << open[i].first->getCourseCode()
                              << "  Session #" << open[i].second->getSessionID()
                              << " " << open[i].second->getTimeSlot() << "\n";
                }

                int pick = readInt("Select session (0 to cancel): ");

                if (pick < 1 || pick > static_cast<int>(open.size())) break;

                // markAttendance throws if closed or already marked
                open[pick - 1].second->markAttendance(getUserID(), "SELF_CHECKIN"); // NOTADDED

                std::cout << "[Success] Checked in to " << open[pick - 1].first->getCourseCode()
                          << " session #" << open[pick - 1].second->getSessionID() << "\n";
                changed = true;
                break;
            }

            // view attendance record
            case 6: {

                if (enrolledCourses.empty()) {
                    std::cout << "You are not enrolled in any course.\n";
                    break;
                }

                for (Course* c : enrolledCourses) {
                    AttendanceRegister* reg = c->getRegister();
                    std::cout << "\n--- " << c->getCourseCode() << " ---\n";

                    auto sessions = reg->getSessions();

                    if (sessions.empty()) {
                        std::cout << "  No sessions held yet.\n";
                        continue;
                    }

                    for (AttendanceSession* s : sessions) {
                        bool present = false;
                        for (const AttendanceRecord& r : s->getRecords()) {
                            if (r.getStudentID() == getUserID()) {
                                present = true;
                                break;
                            }
                        }
                        std::cout << "  Session #" << s->getSessionID() << " " << s->getTimeSlot()
                                  << "  " << (present ? "PRESENT" : (s->isSessionOpen() ? "-" : "ABSENT"))
                                  << (s->isSessionOpen() ? "  (open)" : "") << "\n";
                    }
                    std::cout << "  Attendance: " << reg->calculateStudentPercentage(getUserID()) << "%\n";
                }
                break;
            }

            default:
                std::cout << "Invalid option. Try again.\n";
            }

            if (changed) {   // save straight away, same as admin/lecturer menu
                ctx.users.save(ctx.usersFile);
                ctx.courses.save(ctx.coursesFile);
            }
        }
        catch (const std::exception& e) {
            std::cout << "[Error] " << e.what() << "\n";
        }
    }
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