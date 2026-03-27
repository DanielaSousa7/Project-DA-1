#ifndef PROJECT_DA_1_DATA_H
#define PROJECT_DA_1_DATA_H

#include <string>
#include <vector>
using namespace std;

struct Submission {
    int id;
    string title;
    string authors;
    string email;
    int primaryDomain;
    int secondaryDomain;
};

struct Reviewer {
    int id;
    string name;
    string email;
    int primaryExpertise;
    int secondaryExpertise;
};

struct Parameters {
    int minReviewsPerSubmission;
    int maxReviewsPerReviewer;
    int primaryReviewerExpertise;
    int secondaryReviewerExpertise;
    int primarySubmissionDomain;
    int secondarySubmissionDomain;
};

struct Control {
    int generateAssignments;
    int riskAnalysis;
    string outputFileName;
};

struct AssignmentData {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters parameters;
    Control control;
    bool hasErrors = false;
};

#endif