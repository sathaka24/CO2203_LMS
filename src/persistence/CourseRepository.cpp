#include "CourseRepository.h"

#include "UserRepository.h"
#include "Student.h"
#include "Lecturer.h"
#include "LectureCourse.h"
#include "LabCourse.h"
#include "ProjectCourse.h"
#include "Timetable.h"
#include "TimeSlot.h"
#include "StorageUtils.h"

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace {

const int COURSE_FIELDS = 9;

Course* createCourse(const string& type, const string& code, const string& title, int credits, int capacity) {
    if (type == "Lecture") {

        return new LectureCourse(code, title, credits, capacity);
    }
    if (type == "Lab") {

        return new LabCourse(code, title, credits, capacity);
    }

    if (type == "Project") {
        
        return new ProjectCourse(code, title, credits, capacity);

    }

    return nullptr;
}

// here is the structure we use in this repo to store our course details
struct PendingLinks {
    Course* course;
    string lecturerID;
    vector<string> prerequisiteCodes;
    vector<string> studentIDs;
    string at;   // "courses.txt line details for error messages
};

}

CourseRepository::CourseRepository(UserRepository* users) : userRepo(users) {}

void CourseRepository::save(const string& filename) {

    if (userRepo == nullptr) {

        throw logic_error("CourseRepository::save - no UserRepository set");
    }

    // here we create a map to store the course code and user ID {'code' : ['name1', 'name2', ...]}
    map<string, vector<string>> enrolled;

    for (Person* p : userRepo->getAll()) {

        Student* s = dynamic_cast<Student*>(p);

        if (s != nullptr) {

            // here we get the student's enrolled courses and search or create on map and pushback the student id.
            for (Course* c : s->getEnrolledCourses()) { // FUNCALT
                
                enrolled[c->getCourseCode()].push_back(storage::checkListItem(s->getUserID()));
            }
        }
    }

    ostringstream buffer;

    buffer << "# TYPE|code|title|credits|capacity|lecturerID|prerequisites|timeslots|students\n"; // here is the data store format of course file

    // here the below items comes from our abstract repository template class
    for (auto& entry : items) {

        const Course* c = entry.second;

        vector<string> prereqCodes;

        for (Course* pre : c->getPrerequisites()) { // FUNCALT

            prereqCodes.push_back(pre->getCourseCode()); // FUNCALT
        }

        // here we store the course time slote details in a string vectors. 
        // each element contain day, start time, end time, location
        vector<string> slotTexts;
        Timetable* tt = c->getTimetable(); // FUNCALT

        if (tt != nullptr) {

            for (TimeSlot* t : tt->getSlots()) { // FUNCALT

                // here we use our util func to join the details to a string 
                slotTexts.push_back(storage::join({ storage::checkListItem(t->getDay()),
                                                    storage::checkListItem(t->getStartTime()),
                                                    storage::checkListItem(t->getEndTime()),
                                                    storage::checkListItem(t->getLocation()) }, ','));
            }
        }

        // here we get the lecturer corrosponding to the course
        Lecturer* lec = c->getLecturer(); // FUNCALT

        string lecturerID;
        if(lec != nullptr){
            lec->getUserID(); //FUNCALT

        }else {

            string();
        }

        // here we write our entry to the output stream
        buffer << storage::checkField(c->getCourseType()) << '|'
               << storage::checkListItem(c->getCourseCode()) << '|'
               << storage::checkField(c->getTitle()) << '|' // FUNCALT
               << c->getCredits() << '|' // FUNCALT
               << c->getCapacity() << '|'
               << storage::checkListItem(lecturerID) << '|'
               << storage::join(prereqCodes, ',') << '|'
               << storage::join(slotTexts, ';') << '|'
               << storage::join(enrolled[c->getCourseCode()], ',') << '\n';
    }

    storage::writeFile(filename, buffer.str());
}

void CourseRepository::load(const string& filename) {

    if (userRepo == nullptr) {

        throw logic_error("CourseRepository::load - no UserRepository set (load users first)");
    }

    ifstream in(filename);

    clear();

    // this vectors we use to store the course details which we load from the course.txt data file
    vector<PendingLinks> pending;

    // here create every course and its time slots
    string line;
    int lineNo = 0;

    while (getline(in, line)) {

        ++lineNo;

        storage::stripCR(line);

        if (line.empty() || line[0] == '#') continue;

        const string at = storage::where(filename, lineNo);
        vector<string> f = storage::split(line, '|');

        if (f.size() != COURSE_FIELDS) {
            throw DataCorruptedException(at + ": expected 9 fields but found " + to_string(f.size()));
        }

        const string& type  = f[0];
        const string& code  = f[1];
        const string& title = f[2];

        if (code.empty()) {
            // TODO: exception
            throw DataCorruptedException(at + ": course code is empty");

        }
        if (get(code) != nullptr) {

            //TODO: exception
            throw DataCorruptedException(at + ": duplicate course code " + code);

        }

        int credits  = storage::toInt(f[3], at);
        int capacity = storage::toInt(f[4], at);

        Course* c = createCourse(type, code, title, credits, capacity);

        // TODO: exception
        add(code, c);

        // Time slots belong to the course itself, so they can be added now.
        for (string& slotText : storage::split(f[7], ';')) {

            // here we store the time slot details --> day, start time, end time, location
            vector<string> s = storage::split(slotText, ',');

            if (s.size() != 4) {
                // TODO: exception
                throw DataCorruptedException(at + ": bad time slot \"" + slotText +
                                             "\" (expected day,start,end,location)");
            }

            c->getTimetable()->addSlot(new TimeSlot(s[0], s[1], s[2], s[3])); // FUNCALT

        }

        PendingLinks temp = {
            c,
            f[5],
            storage::split(f[6],','), 
            storage::split(f[8], ','), 
            at
        };

        pending.push_back(temp);
    }

    // ---- Pass 2: turn the stored IDs back into pointers --------------------
    for (const PendingLinks& link : pending) {

        if (!link.lecturerID.empty()) {

            // here we get the pointer to the lecture object using the id
            Lecturer* lec = dynamic_cast<Lecturer*>(userRepo->get(link.lecturerID)); // FUNCALT

            if (lec == nullptr) {
                //TODO: exception
                throw DataCorruptedException(link.at + ": \"" + link.lecturerID +"\" is not a known lecturer");

            }

            // here we assign the got pointer to the course
            link.course->assignLecturer(lec); // FUNCALT

        }

        for (const string& preCode : link.prerequisiteCodes) {

            // here we get a course pointer by it's course code
            Course* pre = get(preCode);
            if (pre == nullptr) {

                // TODO: exception
                throw DataCorruptedException(link.at + ": prerequisite \"" + preCode +"\" does not exist");

            }

            // here we add that pointet to the course
            link.course->addPrerequisite(pre); // FUNCALT
        }

        for (const string& studentID : link.studentIDs) {

            // here we get the enrolled students to the course. 
            // then what we do is we add the course pointer to student object. so each student's course vector fill by this when loading
            Student* s = dynamic_cast<Student*>(userRepo->get(studentID));

            if (s == nullptr) {

                throw DataCorruptedException(link.at + ": \"" + studentID +"\" is not a known student");
            }

            s->addCourse(link.course); // FUNCALT
        }
    }
}
