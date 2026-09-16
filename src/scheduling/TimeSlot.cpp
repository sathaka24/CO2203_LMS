#include "scheduling/TimeSlot.h"
#include <sstream>
#include <algorithm>

TimeSlot::TimeSlot() 
    : day("Monday"), startTime("09:00"), endTime("10:00"), location("TBD") {}

TimeSlot::TimeSlot(const std::string& d, const std::string& start, const std::string& end, const std::string& loc)
    : day(d), startTime(start), endTime(end), location(loc) {}

// Helper to convert "HH:MM" time strings into minutes from midnight
static int convertToMinutes(const std::string& timeStr) {
    int hours = 0;
    int minutes = 0;
    char colon = ':';
    std::stringstream ss(timeStr);
    ss >> hours >> colon >> minutes;
    return (hours * 60) + minutes;
}

bool TimeSlot::operator==(const TimeSlot& other) const {
    return (day == other.day && 
            startTime == other.startTime && 
            endTime == other.endTime && 
            location == other.location);
}

bool TimeSlot::overlaps(const TimeSlot& other) const {
    if (day != other.day) {
        return false;
    }

    int startA = convertToMinutes(startTime);
    int endA   = convertToMinutes(endTime);
    int startB = convertToMinutes(other.startTime);
    int endB   = convertToMinutes(other.endTime);

    // Overlap condition: Max(StartA, StartB) < Min(EndA, EndB)
    return (std::max(startA, startB) < std::min(endA, endB));
}

std::string TimeSlot::getDay() const { return day; }
std::string TimeSlot::getStartTime() const { return startTime; }
std::string TimeSlot::getEndTime() const { return endTime; }
std::string TimeSlot::getLocation() const { return location; }

std::ostream& operator<<(std::ostream& os, const TimeSlot& ts) {
    os << "[" << ts.day << " " << ts.startTime << "-" << ts.endTime << " @ " << ts.location << "]";
    return os;
}