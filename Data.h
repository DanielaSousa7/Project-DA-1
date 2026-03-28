/**
* @file Data.h
 * @brief Data structures used in the review assignment problem.
 */

#ifndef PROJECT_DA_1_DATA_H
#define PROJECT_DA_1_DATA_H

#include <string>
#include <vector>
using namespace std;

/**
 * @brief Represents a submission.
 */
struct Submission {
    int id;               ///< Submission ID
    string title;         ///< Submission title
    string authors;       ///< Submission authors
    string email;         ///< Contact email
    int primaryDomain;    ///< Primary domain
    int secondaryDomain;  ///< Secondary domain
};

/**
 * @brief Represents a reviewer.
 */
struct Reviewer {
    int id;                  ///< Reviewer ID
    string name;             ///< Reviewer name
    string email;            ///< Reviewer email
    int primaryExpertise;    ///< Primary expertise domain
    int secondaryExpertise;  ///< Secondary expertise domain
};

/**
 * @brief Stores the problem parameters.
 */
struct Parameters {
    int minReviewsPerSubmission;      ///< Minimum reviews per submission
    int maxReviewsPerReviewer;        ///< Maximum reviews per reviewer
    int primaryReviewerExpertise;     ///< Weight of primary reviewer expertise
    int secondaryReviewerExpertise;   ///< Weight of secondary reviewer expertise
    int primarySubmissionDomain;      ///< Weight of primary submission domain
    int secondarySubmissionDomain;    ///< Weight of secondary submission domain
};

/**
 * @brief Stores the control settings.
 */
struct Control {
    int generateAssignments;  ///< Assignment generation mode
    int riskAnalysis;         ///< Risk analysis mode
    string outputFileName;    ///< Output file name
};

/**
 * @brief Stores all parsed input data.
 */
struct AssignmentData {
    vector<Submission> submissions;  ///< List of submissions
    vector<Reviewer> reviewers;      ///< List of reviewers
    Parameters parameters;           ///< Problem parameters
    Control control;                 ///< Control settings
    bool hasErrors = false;          ///< Indicates whether input errors were found
};

#endif