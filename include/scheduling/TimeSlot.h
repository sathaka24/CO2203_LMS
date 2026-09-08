#ifndef TIME_SLOT_H
#define TIME_SLOT_H

#include <string>

// Owner: M2
// Used by: M1 (Lecturer::openAttendanceSession parameter), M3 (CourseRepository persistence).
class TimeSlot {
private:
    std::string day;
    std::string startTime;
    std::string endTime;
    std::string location;

public:
    TimeSlot(std::string d, std::string start, std::string end, std::string loc);

    bool operator==(const TimeSlot& other) const;   
    bool overlaps(const TimeSlot& other) const;     

    std::string getDay() const;
    std::string getStartTime() const;
    std::string getEndTime() const;
    std::string getLocation() const;
};

#endif 
