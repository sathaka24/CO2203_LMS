#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <map>
#include <string>
#include <vector>

// Owner: M3
// Used by: M3 (UserRepository, CourseRepository).

// here we use template
template <typename T>
class Repository {
protected:
    std::map<std::string, T*> items;

public:
    virtual ~Repository() {
        for (auto& pair : items) delete pair.second;
    }

    virtual void save(const std::string& filename) = 0;
    virtual void load(const std::string& filename) = 0;
};

#endif // REPOSITORY_H
