#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>

/**
 * @brief Represents a paper submission to the conference.
 */
struct Submission {
    int id;                  ///< Unique paper identifier
    std::string title;       ///< Paper title
    std::string authors;     ///< Author name(s)
    std::string email;       ///< Contact e-mail
    int primaryTopic;        ///< Primary topic identifier
    int secondaryTopic;      ///< Secondary topic identifier (0 if undefined)
};

/**
 * @brief Represents a reviewer in the conference.
 */
struct Reviewer {
    int id;                  ///< Unique reviewer identifier
    std::string name;        ///< Reviewer name
    std::string email;       ///< Reviewer e-mail
    int primaryExpertise;    ///< Primary area of expertise
    int secondaryExpertise;  ///< Secondary area of expertise (0 if undefined)
};

/**
 * @brief Problem parameters read from the #Parameters section.
 */
struct Parameters {
    int minReviewsPerSubmission  = 1; ///< Minimum reviews each submission must receive
    int maxReviewsPerReviewer    = 1; ///< Maximum reviews a reviewer can do
    int primaryReviewerExpertise = 1; ///< Weight/flag for primary reviewer expertise
    int secondaryReviewerExpertise = 0; ///< Weight/flag for secondary reviewer expertise
    int primarySubmissionDomain  = 1; ///< Weight/flag for primary submission domain
    int secondarySubmissionDomain = 0; ///< Weight/flag for secondary submission domain
};

/**
 * @brief Control parameters read from the #Control section.
 */
struct Control {
    int generateAssignments = 0;           ///< Assignment mode (0-3)
    int riskAnalysis        = 0;           ///< Risk analysis mode (0 = none, K > 0)
    std::string outputFileName = "output.csv"; ///< Output file name
};

/**
 * @brief Container for all data parsed from a .csv input file.
 */
struct ProblemInstance {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters              params;
    Control                 control;
};

#endif // DATA_STRUCTURES_H
