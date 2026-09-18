#include "domain/Lecturer.h"
#include "domain/Course.h"
#include "domain/Student.h"
#include "attendance/AttendanceSession.h"
#include "attendance/AttendanceRegister.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "scheduling/TimeSlot.h"
#include "attendance/QRCodeCapture.h"
#include "attendance/FileReplayCapture.h"
#include "scheduling/Timetable.h"
#include "exception/Exceptions.h"
#include "app/SystemContext.h"
#include "persistence/StorageUtils.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <string>
#include <ctime>

struct SystemContext;

Lecturer::Lecturer(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)) {}

Lecturer::~Lecturer() = default;

void Lecturer::showMenu(SystemContext& ctx) {

    // here we use lambda function get the whole input line (same style as admin menu)
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

    // lecturer pick one of their own courses (returns nullptr on cancel)
    auto pickCourse = [&]() -> Course* {

        if (assignedCourses.empty()) {
            std::cout << "You have no assigned courses.\n";
            return nullptr;
        }
        for (int i = 0; i < assignedCourses.size(); ++i) {

            std::cout << "  " << (i + 1) << ". " << *assignedCourses[i] << "\n";
        }
        int choice = readInt("Select course (0 to cancel): ");

        if (choice < 1 || choice > static_cast<int>(assignedCourses.size())) {
            return nullptr;
        }
        return assignedCourses[choice - 1];
    };

    // prints all sessions of a course; returns false if there are not anything
    auto listSessions = [](Course* c) {

        auto sessions = c->getRegister()->getSessions();

        if (sessions.empty()) {
            std::cout << "No sessions for " << c->getCourseCode() << " yet.\n";
            return false;
        }
        for (AttendanceSession* s : sessions) {
            std::cout << "  Session #" << s->getSessionID() << " " << s->getTimeSlot()
                      << (s->isSessionOpen() ? "  [OPEN]" : "  [CLOSED]")
                      << "  records: " << s->getRecords().size() << "\n";
        }
        return true;
    };

    // finds the lecturer currently open session (newest first)
    auto findOpenSession = [&]() -> std::pair<Course*, AttendanceSession*> {

        for (Course* c : assignedCourses) {
            
            auto sessions = c->getRegister()->getSessions();

            for (auto it = sessions.rbegin(); it != sessions.rend(); ++it) {

                if ((*it)->isSessionOpen()) {

                    return {c, *it};
                }
            }
        }
        return {nullptr, nullptr};
    };

    // our main loop
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "           LECTURER DASHBOARD           \n";
        std::cout << " Welcome, " << getName() << " (" << getUserID() << ")\n";
        std::cout << "========================================\n";
        std::cout << " 1. View Assigned Courses\n";
        std::cout << " 2. View Course Enrolment List\n";
        std::cout << " 3. Open Attendance Session\n";
        std::cout << " 4. Close Attendance Session\n";
        std::cout << " 5. Record Attendance Correction\n";
        std::cout << " 6. View Attendance Statistics\n";
        std::cout << " 0. Logout\n";
        std::cout << "========================================\n";


        try {
            int choice = readInt("Select option: ");
            bool changed = false;

            switch (choice) {

            case 0:
                std::cout << "Logging out...\n";
                return;

            // view assigned course
            case 1: {
                if (assignedCourses.empty()) {
                    std::cout << "You have no assigned courses.\n";
                }
                for (Course* c : assignedCourses) {
                    std::cout << "  " << *c << "\n";
                }
                break;
            }

            // view enrolled list
            case 2: {
                Course* c = pickCourse();
                if (c) {
                    viewEnrolmentList(c);
                }
                break;
            }

            // open attendece session
            // TODO:
            case 3: {

                auto [openCourse, openSession] = findOpenSession();

                if (openSession) {

                    throw AttendanceException("Session #" + std::to_string(openSession->getSessionID()) +
                                                " for " + openCourse->getCourseCode() +
                                                " is still open. Close it first (option 4)");
                    break;
                }

                Course* c = pickCourse();
                if (!c) break;

                std::string day   = readLine("Day (e.g. Monday): ");
                std::string start = readLine("Start time (HH:MM): ");
                std::string end   = readLine("End time (HH:MM): ");
                std::string loc   = readLine("Location: ");
                int duration      = readInt("Active duration (minutes): ");

                AttendanceSession* session = openAttendanceSession(c, TimeSlot(day, start, end, loc), duration);

                if (!session) {
                    break;
                }

                std::cout << "Capture method:\n"
                          << "  1. QR code\n"
                          << "  2. Manual entry\n"
                          << "  0. Leave session open (capture later)\n";

                int method = readInt("Select: ");

                if (method == 1) {
        
                    std::string hash = c->getCourseCode() + "-" + std::to_string(session->getSessionID()) + "-" + std::to_string(std::time(nullptr) % 10000);

                    QRCodeCapture qr(c->getCourseCode(), session->getSessionID(), duration);

                    session->setCaptureMechanism(&qr); // here we set our capture method as qr code

                    session->runCapture(); // here we start the capturing

                    session->setCaptureMechanism(nullptr); // qr dies at end of this block

                    changed = true;
                }
                else if (method == 2) {
                       FileReplayCapture fc(readLine("Replay file path: "));
                        session->setCaptureMechanism(&fc);
                        session->runCapture();
                        session->setCaptureMechanism(nullptr);

                        changed = true;


                }
                break;
            }

            // close the currentl opened session
            case 4: {

                auto [c, session] = findOpenSession();

                if (!session) {
                    std::cout << "You have no open attendance session.\n";
                    break;
                }

                std::cout << "Closing session #" << session->getSessionID()
                          << " for " << c->getCourseCode() << " " << session->getTimeSlot() << "\n";
                          
                closeAttendanceSession(c, session->getSessionID());
                changed = true;
                break;
            }

            // attendece correction
            case 5: {
                Course* c = pickCourse();
                if (!c || !listSessions(c)) {
                    break;
                }
                int id             = readInt("Session ID: ");
                std::string sID    = readLine("Student ID: ");
                std::string reason = readLine("Reason: ");
                std::string status = readLine("status (PRESENT/ABSENT): ");
                recordCorrection(c, id, sID, reason, status);
                changed = true;
                break;
            }

            // attendece statistics
            case 6: {
                Course* c = pickCourse();
                if (!c) break;
                AttendanceRegister* reg = c->getRegister();

                std::cout << "\n--- Attendance for " << c->getCourseCode() << " ---\n";
                if (!listSessions(c)) break;

                for (Student* s : c->getEnrolledStudents()) {
                    std::cout << "  " << s->getUserID() << "  " << s->getName() << ": "
                              << reg->calculateStudentPercentage(s->getUserID()) << "%\n";
                }

                break;
            }

            default:
                std::cout << "Invalid option. Try again.\n";
            }

            if (changed) {   // save straight away, same as admin menu
                ctx.users.save(ctx.usersFile);
                ctx.courses.save(ctx.coursesFile);
            }

        }
        catch (const std::exception& e) {
            std::cout << "[Error] " << e.what() << "\n";
        }
    }
}

std::vector<Course*> Lecturer::getAssignedCourses() const {
    return assignedCourses;
}

void Lecturer::viewEnrolmentList(Course* c) const {
    if (!c) {
        throw std::invalid_argument("Course pointer is null");
    }
    
    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        throw std::invalid_argument("You are not the assigned lecturer for course " +
                                    c->getCourseCode());
    }

    const std::vector<Student*>& students = c->getEnrolledStudents();

    std::cout << "\n--- Enrolment List for " << c->getCourseCode() << " ---\n";
    std::cout << *c << "\n";
    std::cout << std::string(40, '-') << "\n";

    if (students.empty()) {
        std::cout << "  No students enrolled.\n";
    }
    for (Student* s : students) {
        std::cout << "  - " << s->getUserID() << "  " << s->getName() << "\n";
    }

    std::cout << std::string(40, '-') << "\n";
    std::cout << "  " << c->getEnrolledCount() << "/" << c->getCapacity() << " enrolled"
              << (c->getEnrolledCount() >= c->getCapacity() ? "  (FULL)" : "") << "\n";
}

AttendanceSession* Lecturer::openAttendanceSession(Course* c, TimeSlot slot, int durationMins) {
    if (!c) {
        std::cerr << "[Error] Course cannot be null.\n";
        return nullptr;
    }

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);
    if (it == assignedCourses.end()) {
        std::cout << "[Access Denied] You cannot open sessions for an unassigned course.\n";
        return nullptr;
    }

    AttendanceRegister* reg = c->getRegister();

    
    if (!reg) {
        std::cerr << "[Error] Course has no AttendanceRegister.\n";
        return nullptr;
    }

    int nextSessionID = static_cast<int>(reg->getSessions().size()) + 1;
    AttendanceSession* session = new AttendanceSession(nextSessionID, slot, c);
    session->openSession(durationMins);
    reg->addSession(session);

    std::cout << "[Success] Opened Attendance Session #" << nextSessionID 
              << " for " << c->getCourseCode() 
              << " (Active duration: " << durationMins << " mins).\n";

    return session;
}

void Lecturer::closeAttendanceSession(Course* c, int sessionID) {
    if (!c || !c->getRegister()) {
        std::cerr << "[Error] Invalid course or register.\n";
        return;
    }

    for (AttendanceSession* session : c->getRegister()->getSessions()) {

        if (session && session->getSessionID() == sessionID) {
            session->closeSession();
            std::cout << "[Success] Attendance Session #" << sessionID << " closed.\n";
            return;
        }
    }

    std::cout << "[Error] Session #" << sessionID << " not found.\n";
}

void Lecturer::recordCorrection(Course* c, int sessionID, std::string studentID, std::string reason, std::string status) {
    if (!c || !c->getRegister()) {
        throw std::invalid_argument("Invalid course or register");
    }

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);

    if (it == assignedCourses.end()) {
        throw std::invalid_argument("You can only correct attendance for your own courses");
    }

    bool enrolled = false;
    for (Student* s : c->getEnrolledStudents()) {
        if (s && s->getUserID() == studentID) {
            enrolled = true;
            break;
        }
    }
    if (!enrolled) {
        throw NotEnrolledException("Student " + studentID + " is not enrolled in " + c->getCourseCode());
    }

    // reason is written to the attendance file
    if (reason.empty()) {
        throw std::invalid_argument("Reason cannot be empty");
    }

    storage::checkField(reason);

    for (char& ch : status) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    if (status != "PRESENT" && status != "ABSENT") {
        throw std::invalid_argument("Status must be PRESENT or ABSENT");
    }

    for (AttendanceSession* session : c->getRegister()->getSessions()) {
        if (session && session->getSessionID() == sessionID) {
            session->addCorrection(studentID, this->getUserID(), reason, status);
            std::cout << "[Success] Correction appended for Student: " << studentID 
                      << " (Lecturer: " << getUserID() << ").\n";
            return;
        }
    }
}

void Lecturer::addAssignedCourse(Course* c) {
    if (c && std::find(assignedCourses.begin(), assignedCourses.end(), c) == assignedCourses.end()) {
        assignedCourses.push_back(c);
    }
}

void Lecturer::removeAssignedCourse(Course* c) {

    auto it = std::find(assignedCourses.begin(), assignedCourses.end(), c);

    if (it != assignedCourses.end()) {
        assignedCourses.erase(it);
    }
}