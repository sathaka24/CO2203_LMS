#include <iostream>
#include <limits>
#include <filesystem>
#include "app/SystemContext.h"
#include "domain/Administrator.h"
#include "persistence/UserRepository.h"
#include "persistence/CourseRepository.h"
#include "scheduling/EnrollmentEngine.h"
#include "exception/Exceptions.h"

int main() {
    std::filesystem::create_directories("data");   // saves fail without this folder

    UserRepository users;
    CourseRepository courses(&users, "data/attendance.txt");
    EnrollmentEngine enrollment(&users, &courses);

    try {

        bool firstRun = false;

        try {
            users.load("data/users.txt");
        } catch (const MissingFileException&) {   // first run only

            firstRun = true;

            users.add("admin", new Administrator("admin", "Admin", "admin"));
        }
        try {
            courses.load("data/courses.txt");
        } catch (const MissingFileException&) {
            // Missing courses file is only acceptable on a genuine first run.
            // If users.txt existed, courses.txt has been moved or deleted and
            // continuing would overwrite it with an empty list on save.
            
            if (!firstRun) throw;
        }

    } catch (const std::exception& e) {

        std::cerr << "Load failed: " << e.what() << "\n";
        return 1;   // don't continue, or the next save overwrites your files
    }

    SystemContext ctx{users, courses, enrollment, "data/users.txt", "data/courses.txt"};

    Person* user = users.login();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (user == nullptr) {
        std::cout << "Invalid ID or password.\n";
        return 1;
    }

    user->showMenu(ctx);

    users.save("data/users.txt");
    courses.save("data/courses.txt");
    return 0;
}
