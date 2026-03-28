/**
 * @file Connection.h
 * @brief Flow network construction and risk analysis functions.
 *
 * Builds a Max-Flow network from a problem instance and runs Edmonds-Karp
 * to determine review assignments. Also provides risk analysis.
 */

#ifndef PROJECT_DA_1_CONNECTION_H
#define PROJECT_DA_1_CONNECTION_H

#include "Data.h"
#include "ReviewAssignment.h"
#include <vector>
using namespace std;

/**
 * @brief Runs the review assignment algorithm using Max-Flow (Edmonds-Karp).
 *
 * Builds a flow network with the following structure:
 * - Source → each submission (capacity = MinReviewsPerSubmission)
 * - Submission → reviewer (capacity = 1, only if domains match)
 * - Reviewer → Sink (capacity = MaxReviewsPerReviewer)
 *
 * Then runs Edmonds-Karp to find the maximum flow, which corresponds
 * to the maximum number of valid review assignments.
 *
 * Time complexity: O(V * E²)
 *
 * @param data The full problem instance.
 * @param excludeReviewerId ID of a reviewer to exclude from the network (-1 = include all).
 * @return A ReviewAssignment containing all assigned paper-reviewer pairs.
 */
ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId = -1);

/**
 * @brief Checks whether a full valid assignment is possible.
 *
 * A full assignment means every paper receives at least MinReviewsPerSubmission reviews.
 *
 * @param data The full problem instance.
 * @param excludeReviewerId ID of a reviewer to exclude (-1 = include all).
 * @return true if all papers can receive the minimum number of reviews.
 */
bool assignmentFeasible(AssignmentData data, int excludeReviewerId = -1);

/**
 * @brief Risk Analysis K=1: identifies critical reviewers.
 *
 * A reviewer is "critical" if their individual absence makes a full
 * assignment impossible. This function tests each reviewer one by one.
 *
 * Only runs if the base assignment (with all reviewers) is feasible.
 *
 * Time complexity: O(R * V * E²) where R = number of reviewers.
 *
 * @param data The full problem instance.
 * @return Sorted list of reviewer IDs whose absence breaks the assignment.
 */
vector<int> riskAnalysis(AssignmentData data);

#endif