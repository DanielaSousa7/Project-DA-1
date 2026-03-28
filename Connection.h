/**
 * @file Connection.h
 * @brief Flow network construction and risk analysis functions.
 *
 * Builds a Max-Flow network from a problem instance and runs Edmonds-Karp to determine review assignments.
 * Provides risk analysis.
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
 * @param data The full problem instance.
 * @param excludeReviewerId ID of a reviewer to exclude from the network (-1 = include all).
 * @return A ReviewAssignment containing all assigned paper-reviewer pairs.
 */
ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId = -1);

/**
 * @brief Checks whether a full valid assignment is possible.
 *
 * @param data The full problem instance.
 * @param excludeReviewerId ID of a reviewer to exclude (-1 = include all).
 * @return true if all papers can receive the minimum number of reviews.
 */
bool assignmentFeasible(AssignmentData data, int excludeReviewerId = -1);

/**
 * @brief Risk Analysis K=1: identifies critical reviewers.
 *
 * @param data The full problem instance.
 * @return Sorted list of reviewer IDs whose absence breaks the assignment.
 */
vector<int> riskAnalysis(AssignmentData data);

#endif