#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <vector>
#include <iostream>
#include "scheduling/TimeSlot.h"

// Owner: M2
// Used by: M1 (Course::courseTimetable, Student::personalTimetable), M3 (CourseRepository).

class Timetable {
private:
    TimeSlot** slots;
    int capacity;
    int count;

public:
    Timetable();
    ~Timetable();
    Timetable(const Timetable& other);                      // copy constructor
    Timetable& operator=(const Timetable& other);          // copy assignment
    Timetable(Timetable&& other) noexcept;                  // move constructor
    Timetable& operator=(Timetable&& other) noexcept;       // move assignment

    void addSlot(TimeSlot* t);
    bool checkClash(const TimeSlot& t) const;           
    std::vector<TimeSlot*> getSlots() const;

    friend std::ostream& operator<<(std::ostream& os, const Timetable& t);
};

#endif
