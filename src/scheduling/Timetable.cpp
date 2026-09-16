#include "scheduling/Timetable.h"
#include <iostream>

void Timetable::resize(int newCapacity) {
    TimeSlot** newSlots = new TimeSlot*[newCapacity];
    for (int i = 0; i < count; ++i) {
        newSlots[i] = slots[i];
    }
    for (int i = count; i < newCapacity; ++i) {
        newSlots[i] = nullptr;
    }
    delete[] slots;
    slots = newSlots;
    capacity = newCapacity;
}

Timetable::Timetable(int cap) 
    : capacity(cap > 0 ? cap : 5), count(0) {
    slots = new TimeSlot*[capacity];
    for (int i = 0; i < capacity; ++i) {
        slots[i] = nullptr;
    }
}

// 1. Destructor
Timetable::~Timetable() {
    for (int i = 0; i < count; ++i) {
        delete slots[i];
    }
    delete[] slots;
}

// 2. Copy Constructor (Deep Copy)
Timetable::Timetable(const Timetable& other) 
    : capacity(other.capacity), count(other.count) {
    slots = new TimeSlot*[capacity];
    for (int i = 0; i < count; ++i) {
        slots[i] = new TimeSlot(*other.slots[i]);
    }
    for (int i = count; i < capacity; ++i) {
        slots[i] = nullptr;
    }
}

// 3. Copy Assignment Operator (Deep Copy with self-assignment protection)
Timetable& Timetable::operator=(const Timetable& other) {
    if (this == &other) return *this;

    for (int i = 0; i < count; ++i) {
        delete slots[i];
    }
    delete[] slots;

    capacity = other.capacity;
    count = other.count;
    slots = new TimeSlot*[capacity];
    for (int i = 0; i < count; ++i) {
        slots[i] = new TimeSlot(*other.slots[i]);
    }
    for (int i = count; i < capacity; ++i) {
        slots[i] = nullptr;
    }

    return *this;
}

// 4. Move Constructor
Timetable::Timetable(Timetable&& other) noexcept 
    : slots(other.slots), capacity(other.capacity), count(other.count) {
    other.slots = nullptr;
    other.count = 0;
    other.capacity = 0;
}

// 5. Move Assignment Operator
Timetable& Timetable::operator=(Timetable&& other) noexcept {
    if (this == &other) return *this;

    for (int i = 0; i < count; ++i) {
        delete slots[i];
    }
    delete[] slots;

    slots = other.slots;
    capacity = other.capacity;
    count = other.count;

    other.slots = nullptr;
    other.count = 0;
    other.capacity = 0;

    return *this;
}

void Timetable::addSlot(const TimeSlot& slot) {
    if (checkClash(slot)) {
        throw std::runtime_error("Timetable clash: Slot overlaps with an existing class!");
    }
    if (count >= capacity) {
        resize(capacity * 2);
    }
    slots[count++] = new TimeSlot(slot);
}

bool Timetable::checkClash(const TimeSlot& t) const {
    for (int i = 0; i < count; ++i) {
        if (slots[i]->overlaps(t)) {
            return true;
        }
    }
    return false;
}

std::vector<TimeSlot*> Timetable::getSlots() const {
    std::vector<TimeSlot*> result;
    for (int i = 0; i < count; ++i) {
        result.push_back(slots[i]);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Timetable& tt) {
    os << "--- Timetable Schedule (" << tt.count << " slots) ---\n";
    for (int i = 0; i < tt.count; ++i) {
        os << "  Slot " << (i + 1) << ": " << *tt.slots[i] << "\n";
    }
    return os;
}