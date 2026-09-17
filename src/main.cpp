
#include <iostream>
#include <limits>
#include "app/SystemContext.h"
#include "domain/Administrator.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "scheduling/EnrollmentEngine.h"

int main() {
    UserRepository users;
    CourseRepository courses(&users);
    EnrollmentEngine enrollment(&users, &courses);

    // Load data (if no users file, create a default admin)
    try {
        users.load("data/users.txt");
    } catch (...) {
        users.add("admin", new Administrator("admin", "Admin", "admin"));
    }
    courses.load("data/courses.txt");

    SystemContext ctx{users, courses, enrollment, "data/users.txt", "data/courses.txt"};

    // Login and show the menu for that user
    Person* user = users.login();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (user == nullptr) {
        std::cout << "Invalid ID or password.\n";
        return 1;
    }

    user->showMenu(ctx);

    // Save before exit
    users.save("data/users.txt");
    courses.save("data/courses.txt");
    return 0;
}
