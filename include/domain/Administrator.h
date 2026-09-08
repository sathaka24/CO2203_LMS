#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <string>
#include "domain/Person.h"

class Course;   // forward declaration - M1

// Owner: M1
// Used by: M3 (UserRepository, main.cpp).

class Administrator : public Person {
public:
    Administrator(std::string id, std::string name, std::string pass);
    ~Administrator() override;

    void showMenu() override;

    Person* createUser(std::string id, std::string name, std::string pass, std::string role);
    void updateUser(std::string id, std::string name, std::string pass);
    void removeUser(std::string id);

    Course* createCourse(std::string code, std::string title, int cred, int cap, std::string type);
    void editCourse(std::string code, std::string title, int cred, int cap);
    void removeCourse(std::string code);

    void generateEnrolmentReport() const;
    void generateEligibilityReport(float threshold) const;
};

#endif
