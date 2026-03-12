#include "Parser.h"
#include <iostream>
using namespace std;

int main() {
    AssignmentData data = parseCSV("test.csv");

    // Mostrar submissions
    cout << "Submissions:" << endl;
    for (auto &s : data.submissions) {
        cout << s.id << ", " << s.title << ", " << s.authors << ", "
             << s.email << ", " << s.primaryDomain << ", " << s.secondaryDomain << endl;
    }

    // Mostrar reviewers
    cout << "\nReviewers:" << endl;
    for (auto &r : data.reviewers) {
        cout << r.id << ", " << r.name << ", " << r.email << ", "
             << r.primaryExpertise << ", " << r.secondaryExpertise << endl;
    }

    // Mostrar parameters
    cout << "\nParameters:" << endl;
    cout << "MinReviewsPerSubmission: " << data.parameters.minReviewsPerSubmission << endl;
    cout << "MaxReviewsPerReviewer: " << data.parameters.maxReviewsPerReviewer << endl;

    // Mostrar control
    cout << "\nControl:" << endl;
    cout << "GenerateAssignments: " << data.control.generateAssignments << endl;
    cout << "RiskAnalysis: " << data.control.riskAnalysis << endl;
    cout << "OutputFileName: " << data.control.outputFileName << endl;

    return 0;
}