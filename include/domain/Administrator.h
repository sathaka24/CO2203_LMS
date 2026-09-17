#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <string>
#include "domain/Person.h"
#include "scheduling/TimeSlot.h"

class Course;   // forward declaration - M1

// Owner: M1
// Used by: M3 (UserRepository, main.cpp).
struct SystemContext;

class Administrator : public Person {

private:

    SystemContext* context = nullptr;   // set by showMenu, used by the other admin functions

public:
    Administrator(std::string id, std::string name, std::string pass);
    ~Administrator() override;

    void showMenu(SystemContext& ctx) override;

    Person* createUser(std::string id, std::string name, std::string pass, std::string role);
    void updateUser(std::string id, std::string name, std::string pass);
    void removeUser(std::string id);

    Course* createCourse(std::string code, std::string title, int cred, int cap, std::string type);
    void editCourse(std::string code, std::string title, int cred, int cap);
    void removeCourse(std::string code);

    void generateEnrolmentReport() const;
    void generateEligibilityReport(float threshold) const;

    void assignLecturerToCourse(std::string code, std::string lecturerID);
    void addCourseTimeSlot(std::string code, const TimeSlot& slot);
    void addCoursePrerequisite(std::string code, std::string prereqCode);
    void removeCoursePrerequisite(std::string code, std::string prereqCode);
};

#endif
