#ifndef PROJECTCOURSE_H
#define PROJECTCOURSE_H

#include "domain/Course.h"

// Owner: M1
// Used by: M3 (CourseRepository factory on load), M1 (Administrator::createCourse).

class ProjectCourse : public Course {
public:
    ProjectCourse(std::string code, std::string title, int cred, int cap);
    ~ProjectCourse() override;

    std::string getCourseType() const override;
    float calculateFinalGrade(std::map<std::string, float> componentScores) const override;
    std::map<std::string, float> getAssessmentBreakdown() const override;
};

#endif
