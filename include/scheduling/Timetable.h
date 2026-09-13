#ifndef TIMETABLE_H
#define TIMETABLE_H

#include "scheduling/TimeSlot.h"
#include <vector>
#include <iostream>

class Timetable {
private:
    TimeSlot** slots;
    int capacity;
    int count;

    void resize(int newCapacity);

public:
    explicit Timetable(int cap = 5);
    
    // Rule of Five
    ~Timetable();
    Timetable(const Timetable& other);
    Timetable& operator=(const Timetable& other);
    Timetable(Timetable&& other) noexcept;
    Timetable& operator=(Timetable&& other) noexcept;

    void addSlot(const TimeSlot& slot);
    bool checkClash(const TimeSlot& t) const;
    std::vector<TimeSlot*> getSlots() const;
    int getCount() const;

    friend std::ostream& operator<<(std::ostream& os, const Timetable& tt);
};

#endif // TIMETABLE_H