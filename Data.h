/**
 * @file Data.h
 * @brief Core data structures used throughout the application.
 *
 * Defines the structs that represent submissions, reviewers, parameters,
 * control settings, and the full problem instance loaded from a CSV file.
 */

#ifndef PROJECT_DA_1_DATA_H
#define PROJECT_DA_1_DATA_H

#include <string>
#include <vector>
using namespace std;

/**
 * @brief Represents a paper submitted to the conference.
 */
struct Submission {
    int id;              ///< Unique paper identifier
    string title;        ///< Paper title
    string authors;      ///< Author name(s)
    string email;        ///< Contact e-mail
    int primaryDomain;   ///< Primary topic area (e.g. 1 = Machine Learning)
    int secondaryDomain; ///< Secondary topic area (-1 if not defined)
};

/**
 * @brief Represents a reviewer in the conference.
 */
struct Reviewer {
    int id;                  ///< Unique reviewer identifier
    string name;             ///< Full name
    string email;            ///< E-mail address
    int primaryExpertise;    ///< Primary area of expertise
    int secondaryExpertise;  ///< Secondary area of expertise (-1 if not defined)
};

/**
 * @brief Problem parameters read from the #Parameters section of the CSV.
 */
struct Parameters {
    int minReviewsPerSubmission;    ///< Minimum number of reviewers each paper must receive
    int maxReviewsPerReviewer;      ///< Maximum number of papers a reviewer can handle
    int primaryReviewerExpertise;   ///< 1 if primary reviewer expertise is active, 0 otherwise
    int secondaryReviewerExpertise; ///< 1 if secondary reviewer expertise is active, 0 otherwise
    int primarySubmissionDomain;    ///< 1 if primary submission domain is active, 0 otherwise
    int secondarySubmissionDomain;  ///< 1 if secondary submission domain is active, 0 otherwise
};

/**
 * @brief Control settings read from the #Control section of the CSV.
 */
struct Control {
    int generateAssignments; ///< Assignment mode: 0=silent, 1=primary only, 2=sub primary+secondary vs rev primary
    int riskAnalysis;        ///< Risk analysis mode: 0=none, 1=single reviewer absence
    string outputFileName;   ///< Name of the output CSV file
};

/**
 * @brief Container for all data parsed from a .csv input file.
 */
struct AssignmentData {
    vector<Submission> submissions; ///< List of all paper submissions
    vector<Reviewer> reviewers;     ///< List of all reviewers
    Parameters parameters;          ///< Problem parameters
    Control control;                ///< Control settings
};

#endif