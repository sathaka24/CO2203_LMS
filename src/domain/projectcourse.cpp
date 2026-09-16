#include "domain/ProjectCourse.h"

ProjectCourse::ProjectCourse(std::string code, std::string title, int cred, int cap)
    : Course(std::move(code), std::move(title), cred, cap) {}

ProjectCourse::~ProjectCourse() = default;

std::string ProjectCourse::getCourseType() const {
    return "Project";
}

std::map<std::string, float> ProjectCourse::getAssessmentBreakdown() const {
    // Project Course: 20% Proposal & Milestones, 50% Project Implementation, 30% Final Defense/Report
    return {
        {"Milestones",     0.20f},
        {"Implementation", 0.50f},
        {"DefenseReport",  0.30f}
    };
}

float ProjectCourse::calculateFinalGrade(std::map<std::string, float> componentScores) const {
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