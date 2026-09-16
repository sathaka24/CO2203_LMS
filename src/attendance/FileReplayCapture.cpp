#include "attendance/FileReplayCapture.h"
#include "exception/Exceptions.h"
#include <iostream>
#include <cctype>

using namespace std;

namespace {

// removes spaces and tabs from both ends
string trim(const string& s) {

    size_t first = s.find_first_not_of(" \t");

    if (first == string::npos) {
        return "";
    }
    size_t last = s.find_last_not_of(" \t");

    return s.substr(first, last - first + 1);
}

}

FileReplayCapture::FileReplayCapture(string path): filePath(move(path)), lineNo(0) {}

FileReplayCapture::~FileReplayCapture() {

    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void FileReplayCapture::beginSession() {

    if (fileStream.is_open()) {
        fileStream.close();
    }

    fileStream.clear();
    fileStream.open(filePath);

    lineNo = 0;

    if (!fileStream) {
        throw MissingFileException("Cannot open attendance replay file: " + filePath);
    }
    cout << " [FILE REPLAY SESSION STARTED] Reading from " << filePath << "\n";
}

// checks one event line and returns the student ID, or throws if the line is bad
string FileReplayCapture::parseEvent(const string& line) const {
    const string at = filePath + " line " + to_string(lineNo);

    size_t bar = line.find('|');

    if (bar == string::npos) {

        throw DataCorruptedException(at + ": expected CHECKIN|<studentID> but got \"" + line + "\"");
    }
    if (line.find('|', bar + 1) != string::npos) {

        throw DataCorruptedException(at + ": too many fields in \"" + line + "\"");
    }

    string eventType = trim(line.substr(0, bar));
    string studentID = trim(line.substr(bar + 1));

    if (eventType != "CHECKIN") {

        throw DataCorruptedException(at + ": unknown event type \"" + eventType + "\"");
    }
    if (studentID.empty()) {

        throw DataCorruptedException(at + ": student ID is empty");
    }
    for (char ch : studentID) {

        if (!isalnum(static_cast<unsigned char>(ch))) {
            throw DataCorruptedException(at + ": invalid student ID \"" + studentID + "\"");
        }
    }
    return studentID;
}

string FileReplayCapture::captureNext() {
    if (!fileStream.is_open()) {
        return "";   // "" tells AttendanceSession::runCapture to stop
    }

    string line;
    while (getline(fileStream, line)) {

        ++lineNo;
        if (!line.empty() && line.back() == '\r') {
            
            line.pop_back();   // Windows line endings

        }

        line = trim(line);

        if (line.empty() || line[0] == '#') {
            
            continue;   // blank or comment line

        }

        return parseEvent(line);   // throws DataCorruptedException if malformed
    }
    return "";   // end of file
}

void FileReplayCapture::endSession() {
    if (fileStream.is_open()) {

        fileStream.close();
    }

    cout << " [FILE REPLAY SESSION CLOSED]\n";
}

string FileReplayCapture::getMethodName() const {
    
    return "FILE_REPLAY";
}