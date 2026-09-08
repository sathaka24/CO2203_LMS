#ifndef LECTURECOURSE_H
#define LECTURECOURSE_H

#include "domain/Course.h"

// Owner: M1
// Used by: M3 (CourseRepository factory on load), M1 (Administrator::createCourse).

class LectureCourse : public Course {
public:
    LectureCourse(std::string code, std::string title, int cred, int cap);
    ~LectureCourse() override;

    std::string getCourseType() const override;
    float calculateFinalGrade(std::map<std::string, float> componentScores) const override;
    std::map<std::string, float> getAssessmentBreakdown() const override;
};

#endif
