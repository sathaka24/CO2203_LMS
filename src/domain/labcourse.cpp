#include "domain/LabCourse.h"

LabCourse::LabCourse(std::string code, std::string title, int cred, int cap)
    : Course(std::move(code), std::move(title), cred, cap) {}

LabCourse::~LabCourse() = default;

std::string LabCourse::getCourseType() const {
    return "Lab";
}

std::map<std::string, float> LabCourse::getAssessmentBreakdown() const {
    // Lab Course emphasizes hands-on practicals: 40% Lab Reports, 30% Practical Exam, 30% Viva
    return {
        {"LabReports",    0.40f},
        {"PracticalExam", 0.30f},
        {"Viva",          0.30f}
    };
}

float LabCourse::calculateFinalGrade(std::map<std::string, float> componentScores) const {
    auto breakdown = getAssessmentBreakdown();
    float finalGrade = 0.0f;
    for (const auto& [component, weight] : breakdown) {
        auto it = componentScores.find(component);
        if (it != componentScores.end()) {
            finalGrade += (it->second * weight);
        }
    }
    return finalGrade;
}