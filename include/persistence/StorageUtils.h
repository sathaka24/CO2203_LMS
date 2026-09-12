#ifndef STORAGE_UTILS_H
#define STORAGE_UTILS_H

// this file just contain some utils for course Repo and user Repo
// all the function are in a namespace to avoid name conflicts

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


namespace storage {

inline std::vector<std::string> split(const std::string& text, char delim) {

    // here we store the splited parts
    std::vector<std::string> parts;

    if (text.empty()){ 
        return parts
    };

    std::string part;
    std::stringstream ss(text);
    while (std::getline(ss, part, delim)) {
        parts.push_back(part);
    }
    if (text.back() == delim) {
        parts.push_back("");   // getline drops a empty field which at the end of the string stream. we use this to get it
    }
    return parts;
}

// here we join the given vector to a string separating by delim value
inline std::string join(const std::vector<std::string>& values, char delim) {
    std::string out;
    for (int i = 0; i < values.size(); ++i) {
        if (i > 0) {

            out += delim;
        }

        out += values[i];
    }
    return out;
}

// A normal field may not contain the field separator or a line break,
// otherwise the file could not be read back correctly.
inline std::string checkField(const std::string& value) {

    if (value.find_first_of("|\r\n") != std::string::npos) {

        throw std::invalid_argument("Cannot save \"" + value +"\": it contains '|' or a line break");
    }
    return value;
}

// Values that appear inside lists (IDs, course codes, time slot parts)
// additionally may not contain ',' or ';'.
inline std::string checkListItem(const std::string& value) {

    if (value.find_first_of("|,;\r\n") != std::string::npos) {

        throw std::invalid_argument("Cannot save \"" + value +"\": it contains one of | , ; or a line break");


    }
    return value;
}

// normally "12" to int 12 is fine. but if string is "12abc" or "" then this function throw an error
inline int toInt(const std::string& text, const std::string& where) {
    try {
        std::size_t used = 0;
        int value = std::stoi(text, &used);
        if (used == text.size()) return value;
    } catch (const std::exception&) {
        // fall through to the throw below
    }

    //TODO: exception
    throw DataCorruptedException(where + ": \"" + text + "\" is not a whole number");
}

// normally files on Windows end lines with "\r\n"; getline leaves the '\r'.
inline void stripCR(std::string& line) {

    if (!line.empty() && line.back() == '\r') {

        line.pop_back(); // since getline remain the "\r" in the string and it is at the end here we pop it to remove

    }
}

// here we use just for formating which line of the text file has error

inline std::string where(const std::string& filename, int lineNo) {

    return filename + " line " + std::to_string(lineNo);


}

// this is the function which writes to the file and save it.
inline void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream out(filename, std::ios::trunc);
    
    // TODO: Add exceptions
    out << content;
    
}

}

#endif
