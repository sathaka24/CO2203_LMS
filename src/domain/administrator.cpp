#include "domain/Administrator.h"
#include "domain/Student.h"
#include "domain/Lecturer.h"
#include "domain/LectureCourse.h"
#include "domain/LabCourse.h"
#include "domain/ProjectCourse.h"
#include "app/SystemContext.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "attendance/AttendanceRegister.h"
#include "attendance/AttendanceSession.h"
#include "scheduling/Timetable.h"
#include "scheduling/TimeSlot.h"
#include "persistence/StorageUtils.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <cctype>

namespace {

// here we convert time in HH:MM format to minutes
int parseTime(const std::string& t) {
    if ((t.size() != 5) || (t[2] != ':') || (!std::isdigit((unsigned char)t[0])) || (!std::isdigit((unsigned char)t[1])) || (!std::isdigit((unsigned char)t[3])) || (!std::isdigit((unsigned char)t[4]))) {

        throw std::invalid_argument("Time \"" + t + "\" must be in HH:MM format");

    }

    int h = std::stoi(t.substr(0, 2));
    int m = std::stoi(t.substr(3, 2));


    if (h > 23 || m > 59) {
        
        throw std::invalid_argument("Time \"" + t + "\" is not a valid time");

    }
    return h * 60 + m;
}

// here we check whether a course has target course as prerequest
bool dependsOn(const Course* from, const Course* target) {

    for (Course* p : from->getPrerequisites()){

        if (p == target || dependsOn(p, target)) {
            return true;
        }

    }
    return false;
}

}

Administrator::Administrator(std::string id, std::string name, std::string pass)
    : Person(std::move(id), std::move(name), std::move(pass)) {}

Administrator::~Administrator() = default;

void Administrator::showMenu(SystemContext& ctx) {
    context = &ctx;

    // here we use lambda function get the whole input line (so names can contain spaces)
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

    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "        ADMINISTRATOR DASHBOARD         \n";
        std::cout << " Welcome, " << getName() << " (" << getUserID() << ")\n";
        std::cout << "========================================\n";
        std::cout << " 1. Create User Account\n";
        std::cout << " 2. Update User Account\n";
        std::cout << " 3. Remove User Account\n";
        std::cout << " 4. Create Course Offering\n";
        std::cout << " 5. Edit Course Details\n";
        std::cout << " 6. Remove Course Offering\n";
        std::cout << " 7. Generate Enrolment Summary Report\n";
        std::cout << " 8. Generate Attendance Eligibility Report\n";
        std::cout << " 9. Assign Lecturer to Course\n";
        std::cout << "10. Add Time Slot to Course\n";
        std::cout << "11. Add Prerequisite to Course\n";
        std::cout << "12. Remove Prerequisite from Course\n";
        std::cout << " 0. Logout\n";
        std::cout << "========================================\n";

        try {
            int choice = readInt("Select option: ");
            bool changed = false;

            switch (choice) {
            case 0:
                std::cout << "Logging out...\n";
                context = nullptr;
                return;

            // Creating a user
            case 1: {
                std::string role = readLine("Role (Student/Lecturer/Admin): ");
                std::string id   = readLine("User ID: ");

                if (ctx.users.get(id) != nullptr) {
                    throw std::invalid_argument("User ID " + id + " already exists");
                }

                std::string name = readLine("Name: ");
                std::string pass = readLine("Password: ");

                Person* p = createUser(id, name, pass, role);

                if (p != nullptr) {
                    ctx.users.add(id, p);   // repository takes ownership
                    std::cout << "[Success] User " << id << " created.\n";
                    changed = true;
                }
                break;
            }
            
            // Update a user details
            case 2: {

                std::string id   = readLine("User ID: ");
                std::string name = readLine("New name: ");
                std::string pass = readLine("New password: ");
                updateUser(id, name, pass);
                changed = true;
                break;
            }

            // Remove user
            case 3: {

                std::string id = readLine("User ID: ");

                if (id == getUserID()) {
                    throw std::invalid_argument("You cannot remove your own account");
                }

                removeUser(id);
                changed = true;
                break;
            }

            // create a course
            case 4: {

                std::string type = readLine("Type (Lecture/Lab/Project): ");
                std::string code = readLine("Course code: ");

                if (ctx.courses.get(code) != nullptr) {
                    throw std::invalid_argument("Course " + code + " already exists");
                }

                std::string title = readLine("Title: ");
                int cred = readInt("Credits: ");
                int cap  = readInt("Capacity: ");
                if (cred <= 0 || cap <= 0) {
                    throw std::invalid_argument("Credits and capacity must be positive");
                }

                Course* c = createCourse(code, title, cred, cap, type);
                if (c != nullptr) {

                    ctx.courses.add(code, c);
                    std::cout << "[Success] Course " << code << " created.\n";
                    changed = true;
                }
                break;
            }

            // Edit the course details
            case 5: {

                std::string code  = readLine("Course code: ");
                std::string title = readLine("New title: ");
                int cred = readInt("New credits: ");
                int cap  = readInt("New capacity: ");

                editCourse(code, title, cred, cap); 
                changed = true;
                break;
            }

            // Remove offering course
            case 6: {

                std::string code = readLine("Course code: ");

                removeCourse(code); 
                changed = true;
                break;
            }

            // Enrollment summery
            case 7:

                generateEnrolmentReport(); 
                break;

            // Eligibility Report
            case 8: {

                std::string t = readLine("Threshold % (Enter for 80): ");
                float threshold = t.empty() ? 80.0f : std::stof(t);

                generateEligibilityReport(threshold); 

                break;
            }

            // assign a lecture to a course
            case 9: {
                std::string code = readLine("Course code: ");
                std::string lec  = readLine("Lecturer ID (Enter to unassign): ");
                assignLecturerToCourse(code, lec);
                changed = true;
                break;
            }

            // Add time slot to a course
            case 10: {
                std::string code  = readLine("Course code: ");
                std::string day   = readLine("Day (e.g. Monday): ");
                std::string start = readLine("Start time (HH:MM): ");
                std::string end   = readLine("End time (HH:MM): ");
                std::string loc   = readLine("Location: ");
                addCourseTimeSlot(code, TimeSlot(day, start, end, loc));
                changed = true;
                break;
            }

            // Add prerequisits to a course
            case 11: {
                std::string code = readLine("Course code: ");
                std::string pre  = readLine("Prerequisite course code: ");
                addCoursePrerequisite(code, pre);
                changed = true;
                break;
            }

            // remove prerequistites 
            case 12: {
                std::string code = readLine("Course code: ");
                std::string pre  = readLine("Prerequisite course code to remove: ");
                removeCoursePrerequisite(code, pre);
                changed = true;
                break;
            }

            default:
                std::cout << "Invalid option. Choose 0-8.\n";
            }

            if (changed) {   // save straight away
                ctx.users.save(ctx.usersFile);
                ctx.courses.save(ctx.coursesFile);
            }
        } catch (const std::exception& e) {
            if (!std::cin) {          // input closed: leave the loop
                context = nullptr;
                return;
            }
            std::cout << "[Error] " << e.what() << "\n";
        }
    }
}

Person* Administrator::createUser(std::string id, std::string name, std::string pass, std::string role) {
    if (role == "Student" || role == "student") {
        return new Student(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Lecturer" || role == "lecturer") {
        return new Lecturer(std::move(id), std::move(name), std::move(pass));
    } else if (role == "Administrator" || role == "Admin" || role == "admin") {
        return new Administrator(std::move(id), std::move(name), std::move(pass));
    }
    std::cerr << "[Error] Unknown user role: " << role << "\n";
    return nullptr;
}

void Administrator::updateUser(std::string id, std::string name, std::string pass) {
    if (context == nullptr) {
        throw std::logic_error("updateUser called outside the admin menu");
    }

    Person* p = context->users.get(id);
    if (p == nullptr) {
        throw std::invalid_argument("User " + id + " not found");
    }

    // id name and password are empty means we leave them as it is. not changing
    //TODO:
    if (!name.empty()) {
        p->setName(name);
    }
    if (!pass.empty()) {
        p->setPassword(pass);
    }

    std::cout << "[Success] User " << id << " updated.\n";
}

void Administrator::removeUser(std::string id) {
    if (context == nullptr) {
        throw std::logic_error("removeUser called outside the admin menu");
    }

    Person* p = context->users.get(id);
    if (p == nullptr) {
        throw std::invalid_argument("User " + id + " not found");
    }
    if (p == this) {
        throw std::invalid_argument("You cannot remove your own account");
    }

    // before deleting we clear every course pointer that refers to this person
    if (Student* s = dynamic_cast<Student*>(p)) {
        for (Course* c : s->getEnrolledCourses()) {
            c->removeStudent(s);
        }
    } else if (Lecturer* l = dynamic_cast<Lecturer*>(p)) {
        for (Course* c : context->courses.getAll()) {
            if (c->getLecturer() == l) {
                c->assignLecturer(nullptr);
            }
        }
    }

    context->users.remove(id);
    std::cout << "[Success] User " << id << " removed.\n";
}

Course* Administrator::createCourse(std::string code, std::string title, int cred, int cap, std::string type) {
    if (type == "Lecture" || type == "lecture") {
        return new LectureCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Lab" || type == "lab") {
        return new LabCourse(std::move(code), std::move(title), cred, cap);
    } else if (type == "Project" || type == "project") {
        return new ProjectCourse(std::move(code), std::move(title), cred, cap);
    }
    std::cerr << "[Error] Unknown course type: " << type << "\n";
    return nullptr;
}

void Administrator::editCourse(std::string code, std::string title, int cred, int cap) {
    if (context == nullptr) {
        throw std::logic_error("editCourse called outside the admin menu");
    }

    Course* c = context->courses.get(code);
    if (c == nullptr) {
        throw std::invalid_argument("Course " + code + " not found");
    }
    if (cred <= 0 || cap <= 0) {
        throw std::invalid_argument("Credits and capacity must be positive");
    }
    if (cap < c->getEnrolledCount()) {
        throw std::invalid_argument("Capacity cannot be less than the " +
                                    std::to_string(c->getEnrolledCount()) +
                                    " students already enrolled");
    }

    if (!title.empty()) {   // empty title means "keep the current one"
        c->setTitle(title);
    }
    c->setCredits(cred);
    c->setCapacity(cap);

    std::cout << "[Success] Course " << code << " updated.\n";
}

void Administrator::removeCourse(std::string code) {
    if (context == nullptr) {
        throw std::logic_error("removeCourse called outside the admin menu");
    }

    Course* c = context->courses.get(code);
    if (c == nullptr) {
        throw std::invalid_argument("Course " + code + " not found");
    }

    // 1. drop every enrolled student (copy the list first)
    std::vector<Student*> students = c->getEnrolledStudents();
    for (Student* s : students) {
        s->removeCourse(c);
    }

    // 2. detach the lecturer
    if (Lecturer* l = c->getLecturer()) {
        l->removeAssignedCourse(c);
        c->assignLecturer(nullptr);
    }

    // 3. remove it from other courses' prerequisite lists
    for (Course* other : context->courses.getAll()) {
        other->removePrerequisite(c);
    }

    // 4. delete it (its timetable and attendance register go with it)
    context->courses.remove(code);
    std::cout << "[Success] Course " << code << " removed.\n";
}


void Administrator::generateEnrolmentReport() const {
    if (context == nullptr) {
        throw std::logic_error("generateEnrolmentReport called outside the admin menu");
    }

    std::cout << "\n=======================================================\n";
    std::cout << "           COURSE ENROLMENT SUMMARY REPORT            \n";
    std::cout << "=======================================================\n";
    std::cout << std::left
              << std::setw(10) << "Code"
              << std::setw(26) << "Title"
              << std::setw(10) << "Type"
              << "Enrolled\n";
    std::cout << std::string(55, '-') << "\n";

    int total = 0;
    for (Course* c : context->courses.getAll()) {
        int n = c->getEnrolledCount();
        total += n;

        std::cout << std::setw(10) << c->getCourseCode()
                  << std::setw(26) << c->getTitle().substr(0, 24)
                  << std::setw(10) << c->getCourseType()
                  << n << "/" << c->getCapacity()
                  << (n >= c->getCapacity() ? "  FULL" : "") << "\n";

        Lecturer* l = c->getLecturer();
        std::cout << "    Lecturer: " << (l ? l->getName() : "Unassigned") << "\n";
        for (Student* s : c->getEnrolledStudents()) {
            std::cout << "      - " << s->getUserID() << "  " << s->getName() << "\n";
        }
    }

    std::cout << std::string(55, '-') << "\n";
    std::cout << "Courses: " << context->courses.getAll().size()
              << " | Total enrolments: " << total << "\n";
}

void Administrator::generateEligibilityReport(float threshold) const {
    if (context == nullptr) {
        throw std::logic_error("generateEligibilityReport called outside the admin menu");
    }
    if (threshold < 0.0f || threshold > 100.0f) {
        throw std::invalid_argument("Threshold must be between 0 and 100");
    }

    std::cout << "\n=======================================================\n";
    std::cout << "         EXAM ATTENDANCE ELIGIBILITY REPORT            \n";
    std::cout << " Threshold: " << std::fixed << std::setprecision(1) << threshold << "%\n";
    std::cout << "=======================================================\n";

    int notEligible = 0;
    for (Course* c : context->courses.getAll()) {
        AttendanceRegister* reg = c->getRegister();

        int held = 0;   // only closed sessions count
        for (AttendanceSession* session : reg->getSessions()) {
            if (!session->isSessionOpen()) {
                ++held;
            }
        }

        std::cout << "\n[" << c->getCourseCode() << "] " << c->getTitle()
                  << " (" << held << " session(s) held)\n";

        if (c->getEnrolledStudents().empty()) {
            std::cout << "    No students enrolled.\n";
            continue;
        }

        for (Student* s : c->getEnrolledStudents()) {
            float pct = reg->calculateStudentPercentage(s->getUserID());
            bool eligible = pct >= threshold;
            if (!eligible) {
                ++notEligible;
            }

            std::cout << "    " << std::left << std::setw(10) << s->getUserID()
                      << std::setw(22) << s->getName()
                      << std::right << std::setw(6) << pct << "%  "
                      << (eligible ? "ELIGIBLE" : "NOT ELIGIBLE") << "\n";
        }
    }

    std::cout << std::left << "\nNot eligible (student/course pairs): " << notEligible << "\n";

    std::cout.unsetf(std::ios::fixed);   // before leave here we fixed formatting on for other menus
}





void Administrator::assignLecturerToCourse(std::string code, std::string lecturerID) {
    if (context == nullptr) {
        throw std::logic_error("assignLecturerToCourse called outside the admin menu");
    }

    Course* c = context->courses.get(code);
    if (c == nullptr) {
        throw std::invalid_argument("Course " + code + " not found");
    }

    if (lecturerID.empty()) {
        c->assignLecturer(nullptr);
        std::cout << "[Success] " << code << " no longer has a lecturer.\n";
        return;
    }

    Lecturer* l = dynamic_cast<Lecturer*>(context->users.get(lecturerID));
    if (l == nullptr) {
        
        throw std::invalid_argument(lecturerID + " is not a lecturer");
    }

    c->assignLecturer(l);

    std::cout << "[Success] " << l->getName() << " assigned to " << code << ".\n";
}

void Administrator::addCourseTimeSlot(std::string code, const TimeSlot& slot) {

    if (context == nullptr) {
        throw std::logic_error("addCourseTimeSlot called outside the admin menu");
    }

    Course* c = context->courses.get(code);

    if (c == nullptr) {
        throw std::invalid_argument("Course " + code + " not found");
    }

    static const char* days[] = {"Monday", "Tuesday", "Wednesday", "Thursday",
                                 "Friday", "Saturday", "Sunday"};
    bool validDay = false;
    for (const char* d : days) {

        if (slot.getDay() == d) {
             validDay = true; 
             break; 
        }
    }

    if (!validDay) {
        throw std::invalid_argument("Day must be one of Monday..Sunday (capitalised)");
    }

    if (parseTime(slot.getStartTime()) >= parseTime(slot.getEndTime()))
        throw std::invalid_argument("Start time must be before end time");

    storage::checkListItem(slot.getLocation());

    if (c->getTimetable()->checkClash(slot))
        throw std::invalid_argument("This slot overlaps an existing slot of " + code);

    for (Student* s : c->getEnrolledStudents()) {
        if (s->getTimetable() && s->getTimetable()->checkClash(slot))
            throw std::invalid_argument("Slot clashes with the timetable of enrolled student " + s->getUserID());
    }

    c->getTimetable()->addSlot(slot);

    for (Student* s : c->getEnrolledStudents()) {
        if (s->getTimetable()) {
            
            s->getTimetable()->addSlot(slot);

        }
    }

    std::cout << "[Success] Added " << slot << " to " << code << ".\n";
}

void Administrator::addCoursePrerequisite(std::string code, std::string prereqCode) {

    if (context == nullptr) {

        throw std::logic_error("addCoursePrerequisite called outside the admin menu");
    }

    Course* c   = context->courses.get(code);
    Course* pre = context->courses.get(prereqCode);
    if (c == nullptr)   {


        throw std::invalid_argument("Course " + code + " not found");
    }
    if (pre == nullptr) {

        throw std::invalid_argument("Course " + prereqCode + " not found");
    }
    if (c == pre) {
        throw std::invalid_argument("A course cannot be its own prerequisite");
    }

    if (dependsOn(pre, c)){

        throw std::invalid_argument(prereqCode + " already depends on " + code + "; adding this would create a loop");
    }

    for (Course* existing : c->getPrerequisites()) {
        if (existing == pre)
            throw std::invalid_argument(prereqCode + " is already a prerequisite of " + code);
    }

    c->addPrerequisite(pre);
    std::cout << "[Success] " << prereqCode << " is now a prerequisite of " << code << ".\n";
}

void Administrator::removeCoursePrerequisite(std::string code, std::string prereqCode) {
    if (context == nullptr) {

        throw std::logic_error("removeCoursePrerequisite called outside the admin menu");

    }

    Course* c   = context->courses.get(code);
    Course* pre = context->courses.get(prereqCode);
    if (c == nullptr)   {
        
        throw std::invalid_argument("Course " + code + " not found");
    }
    if (pre == nullptr) {
        throw std::invalid_argument("Course " + prereqCode + " not found");

    }

    c->removePrerequisite(pre);

    std::cout << "[Success] Removed " << prereqCode << " from the prerequisites of " << code << ".\n";
}