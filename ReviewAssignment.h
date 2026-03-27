/**
* @file ReviewAssignment.h
 * @brief Data structures for representing the result of a review assignment.
 */

#ifndef PROJECT_DA_1_REVIEWASSIGNMENT_H
#define PROJECT_DA_1_REVIEWASSIGNMENT_H

#include <vector>

/**
 * @brief Represents a single review assignment between a paper and a reviewer.
 */
struct ReviewAssignmentItem {
    int submissionId; ///< ID of the paper being reviewed
    int reviewerId;   ///< ID of the reviewer assigned to it
    int matchDomain;  ///< The domain that made the match possible
};

/**
 * @brief Holds the full result of running the assignment algorithm.
 *
 * Contains two views of the same assignments:
 * - submissionAssignments: sorted by paper ID
 * - reviewerAssignments: sorted by reviewer ID
 */
struct ReviewAssignment {
    std::vector<ReviewAssignmentItem> submissionAssignments; ///< Assignments sorted by paper ID
    std::vector<ReviewAssignmentItem> reviewerAssignments;   ///< Assignments sorted by reviewer ID
    int totalAssignments = 0; ///< Total number of review assignments made
};

#endif