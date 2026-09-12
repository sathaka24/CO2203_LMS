#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <map>
#include <string>
#include <vector>

using namespace std;
// Owner: M3
// Used by: M3 (UserRepository, CourseRepository).

// here we use template
// other two classes are inherited from this repo class. T is either person or course
template <typename T>
class Repository {
protected:

    // here we use map of times. unlike vectors maps use binary tree data structure. which is faster in seaching
    map<string, T*> items;

    // below function we use to clear the above allocated map
    void clear() {

        // this loop clear the memory allocated in heap
        for(auto& item : items){

            delete item.second;
        }

        items.clear(); // here we remove the elements in map
    }

public:
    Repository() {};

    virtual ~Repository() {
        for (auto& pair : items) delete pair.second;
    }

    void add(string id, T* item){
        // TODO: Exceptions
        items[id] = item
    }

    T* get(string id){

        auto item = items.find(id);

        if (item != items.end())
        {
            return item->second;
        } else {
            return nullptr;
        }
        
    }

    vector<T*> getAll(){

        vector<T*> objects;
        objects.reserve(items.size()); // here we do reservation before. cuz it improve the performance by avoiding reinitializing every time push packing
        for(auto& itme : times){
            objects.push_back(item.second);
        }

        return objects;
    }

    void remove(string id) {

        auto item = items.find(id);
        if (item != items.end())
        {
            delete item->second;
        } else {

            // TODO: Need to add exception
            cout << "Item not found to remove\n";
        }

        items.erase(id);


        
    }

    // below two functions are pure virtual functions which implement in user and course repos
    virtual void save(const string& filename) = 0;
    virtual void load(const string& filename) = 0;
};

#endif // REPOSITORY_H
