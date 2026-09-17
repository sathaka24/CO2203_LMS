#include "domain/Person.h"

// Initialize static member tracking total users across the system
int Person::totalUsers = 0;

Person::Person(std::string id, std::string name, std::string pass)
    : userID(std::move(id)), name(std::move(name)), password(std::move(pass)) {
    ++totalUsers;
}

Person::~Person() {
    --totalUsers;
}

std::string Person::getUserID() const {
    return userID;
}

std::string Person::getName() const {
    return name;
}

bool Person::authenticate(std::string pass) const {
    return password == pass;
}

int Person::getTotalUsers() {
    return totalUsers;
}

std::ostream& operator<<(std::ostream& os, const Person& p) {
    os << "[ID: " << p.userID << ", Name: " << p.name << "]";
    return os;
}

void Person::setName(const std::string& newName) { 
    name = newName; 
}
void Person::setPassword(const std::string& newPass) { 
    password = newPass; 
}