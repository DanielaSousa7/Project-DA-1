/**
 * @file ReviewAssignment.h
 * @brief Defines the structures used to store review assignments.
 */

#ifndef PROJECT_DA_1_REVIEWASSIGNMENT_H
#define PROJECT_DA_1_REVIEWASSIGNMENT_H

#include <vector>

/**
 * @brief Represents one assignment between a submission and a reviewer.
 */
struct ReviewAssignmentItem {
    int submissionId; ///< Submission ID
    int reviewerId;   ///< Reviewer ID
    int matchDomain;  ///< Matching domain
};

/**
 * @brief Stores the final result of the review assignment process.
 */
struct ReviewAssignment {
    std::vector<ReviewAssignmentItem> submissionAssignments; ///< Assignments by submission
    std::vector<ReviewAssignmentItem> reviewerAssignments;   ///< Assignments by reviewer
    int totalAssignments = 0; ///< Total number of assignments
};

#endif