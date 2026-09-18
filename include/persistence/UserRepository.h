#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <string>
#include "persistence/Repository.h"
#include "domain/Person.h"

// Owner: M3
// Used by: M1 (Administrator::showMenu), M2 (EnrollmentEngine), M3 (main.cpp).

class UserRepository : public Repository<Person> {
public:

    void save(const std::string& filename) override;   
    void load(const std::string& filename) override;   

    // login function use at the begining to login to the system. this will call at the main
    Person* login(); 

    // "Student" -> "S", "Lecturer" -> "L", "Admin" -> "A"; throws on anything else
    std::string rolePrefix(const std::string& role);

    // next free ID for that role like S0003. S for student
    std::string nextUserID(const std::string& role);
};

#endif
