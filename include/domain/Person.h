#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

// Owner: M1
// Used by: M3 (UserRepository, main.cpp) via Person*.

class Person {
protected:
    std::string userID;
    std::string name;
    std::string password;

private:
    static int totalUsers;  

public:
    Person(std::string id, std::string name, std::string pass);
    virtual ~Person();

    std::string getUserID() const;
    std::string getName() const;
    bool authenticate(std::string pass) const;

    virtual void showMenu() = 0;          // pure virtual so the Person is abstract class

    static int getTotalUsers();

    friend std::ostream& operator<<(std::ostream& os, const Person& p);
};

#endif
