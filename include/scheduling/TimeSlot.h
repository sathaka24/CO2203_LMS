#ifndef TIME_SLOT_H
#define TIME_SLOT_H

#include <string>
#include <iostream>

class TimeSlot {
private:
    std::string day;
    std::string startTime;
    std::string endTime;
    std::string location;

public:
    // 1. Default constructor
    TimeSlot();

    // 2. Parameterized constructor with const std::string&
    TimeSlot(const std::string& d, const std::string& start, const std::string& end, const std::string& loc);

    bool operator==(const TimeSlot& other) const;   
    bool overlaps(const TimeSlot& other) const;     

    std::string getDay() const;
    std::string getStartTime() const;
    std::string getEndTime() const;
    std::string getLocation() const;

    friend std::ostream& operator<<(std::ostream& os, const TimeSlot& ts);
};

#endif // TIME_SLOT_H