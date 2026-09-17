#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

// Owner: M1
// Used by: M3 (UserRepository, main.cpp) via Person*.

struct SystemContext;

class Person {

    friend class UserRepository;
    
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

    void setName(const std::string& newName);
    void setPassword(const std::string& newPass);

    bool authenticate(std::string pass) const;

    virtual void showMenu(SystemContext& ctx) = 0;          // pure virtual so the Person is abstract class

    static int getTotalUsers();

    friend std::ostream& operator<<(std::ostream& os, const Person& p);
};

#endif
