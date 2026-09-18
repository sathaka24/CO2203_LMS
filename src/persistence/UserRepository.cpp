#include "persistence/UserRepository.h"

#include "domain/Student.h"
#include "domain/Lecturer.h"
#include "domain/Administrator.h"
#include "persistence/StorageUtils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

// here we use namespace so below functions and variables and stuffs only visible to this .cpp file
// so no coflict happen even header includes to other file
namespace {

const int USER_FIELDS = 4;
const int STUDENT_FIELDS = 5;

// The file has to record WHICH subclass each user is, otherwise we could not
// rebuild the right object on load. dynamic_cast tells us the real type.
string roleOf(const Person* p) {
    if (dynamic_cast<const Student*>(p))       return "STUDENT";
    if (dynamic_cast<const Lecturer*>(p))      return "LECTURER";
    if (dynamic_cast<const Administrator*>(p)) return "ADMIN";
    throw logic_error("UserRepository: unknown Person subclass for " + p->getUserID());
}

// Factory: turns the ROLE text back into the right kind of object.
Person* createPerson(const string& role, const string& id,const string& name, const string& pass) {
    if (role == "STUDENT")  {
        return new Student(id, name, pass);
    }

    if (role == "LECTURER") {
        return new Lecturer(id, name, pass);
    }

    if (role == "ADMIN")    {
        return new Administrator(id, name, pass);

    }

    return nullptr;
}

}

/*
    our saving format of users

    # ROLE|userID|name|password|completedCourses
    STUDENT|S1|Alice|pw|CO1010,CO1020
    STUDENT|S2|Nimal|pw|
    LECTURER|L1|Bob|pw|
    ADMIN|A1|Root|pw|


*/


void UserRepository::save(const string& filename) {
    ostringstream buffer;
    buffer << "# ROLE|userID|name|password|completedCourses\n";

    for (const auto& entry : items) {
        const Person* p = entry.second;

        vector<string> completed;

        // here we check whether the entity is a student. cuz students only has completed course field
        if (const Student* s = dynamic_cast<const Student*>(p)) {

            for (const string& code : s->getCompletedCourses()) {
                
                completed.push_back(storage::checkListItem(code));
            }
        }

        buffer << roleOf(p) << '|'
               << storage::checkListItem(p->getUserID()) << '|'
               << storage::checkField(p->getName()) << '|'
               << storage::checkField(p->password) << '|'
               << storage::join(completed, ',')
               << '\n';
    }

    storage::writeFile(filename, buffer.str());
}

void UserRepository::load(const string& filename) {
    ifstream in(filename);
    if (!in) {

        throw MissingFileException("User file not found: " + filename);
    }

    clear();   // replace whatever is in memory with the contents of the file

    string line;
    int lineNo = 0;

    while (getline(in, line)) {
        ++lineNo;
        storage::stripCR(line);
        if (line.empty() || line[0] == '#') continue;

        const string at = storage::where(filename, lineNo); // this part only use for error detection


        vector<string> f = storage::split(line, '|');
        if (f.size() != USER_FIELDS && f.size() != STUDENT_FIELDS) {

            throw DataCorruptedException(at + ": expected 4 or 5 fields but found " + std::to_string(f.size()));
        }

        const string& role = f[0];
        const string& id   = f[1];
        const string& name = f[2];
        const string& pass = f[3];

        if (id.empty()) {
            throw DataCorruptedException(at + ": user ID is empty");
        }
        if (get(id) != nullptr) {
            throw DataCorruptedException(at + ": duplicate user ID " + id);
        }

        Person* p = createPerson(role, id, name, pass);
        if (p == nullptr) {
            throw DataCorruptedException(at + ": unknown role \"" + role + "\"");
        }

        if (f.size() == STUDENT_FIELDS && !f[4].empty()) {

            Student* s = dynamic_cast<Student*>(p);

            if (s == nullptr) {
                delete p;   // not added to the map yet, so free it ourselves
                throw DataCorruptedException(at + ": only students can have completed courses");
            }

            for (const string& code : storage::split(f[4], ',')) {

                if (code.empty()) {
                    delete p;
                    throw DataCorruptedException(at + ": empty course code in completed list");
                }

                s->addCompletedCourse(code);
            }

        }
        add(id, p);
    }

}

Person* UserRepository::login() {

    string id;
    string pass;

    cout << "Enter the id: ";
    cin >> id;
    cout << "Enter the password: ";
    cin >> pass;


    Person* p = get(id);
    
    if(p != nullptr && p->authenticate(pass)){ 

        return p;

    } else{

        return nullptr;
    }
}

std::string UserRepository::rolePrefix(const std::string& role) {
    if (role == "Student"  || role == "student")  {
        return "S";
    }
    if (role == "Lecturer" || role == "lecturer") {
        return "L";
    }
    if (role == "Administrator" || role == "Admin" || role == "admin") {
        return "A";
    }
    throw std::invalid_argument("Unknown user role: " + role);
}

std::string UserRepository::nextUserID(const std::string& role) {

    const std::string prefix = rolePrefix(role);
    int highest = 0;

    // here we iterate through each person and get id
    for (const auto& entry : items) {

        const std::string& id = entry.first;

        if (id.size() != 5) continue; // check format of id. we use S0001, L0001, A0001

        if (id.compare(0, 1, prefix) != 0) continue; // this line allows only to continue on specific role only. otherwis skip to next person

        const std::string digits = id.substr(prefix.size());

        if (digits.find_first_not_of("0123456789") != std::string::npos) continue; // here we again check whether our substring is a number

        highest = std::max(highest, std::stoi(digits)); // get highest digit
    }

    std::ostringstream os;
    os << prefix << std::setw(4) << std::setfill('0') << (highest + 1);
    return os.str();
}