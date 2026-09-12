#include "UserRepository.h"

#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "StorageUtils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

// here we use namespace so below functions and variables and stuffs only visible to this .cpp file
// so no coflict happen even header includes to other file
namespace {

const int USER_FIELDS = 4;

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


void UserRepository::save(const string& filename) {
    ostringstream buffer;
    buffer << "# ROLE|userID|name|password\n";

    for (const auto& entry : items) {
        const Person* p = entry.second;
        buffer << roleOf(p) << '|'
               << storage::checkListItem(p->getUserID()) << '|'
               << storage::checkField(p->getName()) << '|'
               << storage::checkField(p->password)       // needs:  in Person
               << '\n';
    }

    storage::writeFile(filename, buffer.str());
}

void UserRepository::load(const string& filename) {
    ifstream in(filename);
    if (!in) {

        // TODO: add exception
        cout << "User file not found: " + filename;
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
        if (f.size() != USER_FIELDS) {

            // TODO: add exception
            cout << at + ": expected 4 fields but found " + to_string(f.size());
        }

        const string& role = f[0];
        const string& id   = f[1];
        const string& name = f[2];
        const string& pass = f[3];

        // TODO: add exceptions
        Person* p = createPerson(role, id, name, pass);
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
    
    //TODO: here person class need to have a authentication function to check password is matching
    if(p != nullptr && p->authenticate(pass)){ 

        return p;

    } else{

        return nullptr;
    }
}