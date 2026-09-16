#include "domain/LectureCourse.h"

LectureCourse::LectureCourse(std::string code, std::string title, int cred, int cap)
    : Course(std::move(code), std::move(title), cred, cap) {}

LectureCourse::~LectureCourse() = default;

std::string LectureCourse::getCourseType() const {
    return "Lecture";
}

std::map<std::string, float> LectureCourse::getAssessmentBreakdown() const {
    // 20% Assignments, 30% Midterm Exam, 50% Final Exam
    return {
        {"Assignments", 0.20f},
        {"Midterm",     0.30f},
        {"FinalExam",   0.50f}
    };
}

float LectureCourse::calculateFinalGrade(std::map<std::string, float> componentScores) const {
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