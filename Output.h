/**
* @file Output.h
 * @brief Function for writing the assignment result to a CSV file.
 */

#ifndef PROJECT_DA_1_OUTPUT_H
#define PROJECT_DA_1_OUTPUT_H

#include "Data.h"
#include "ReviewAssignment.h"
#include <string>
using namespace std;

/**
 * @brief Writes the review assignment result to a CSV output file.
 *
 * The output file contains:
 * - `#SubmissionId,ReviewerId,Match` — one line per assignment, sorted by paper ID
 * - `#ReviewerId,SubmissionId,Match` — same assignments, sorted by reviewer ID
 * - `#Total: N` — total number of assignments
 * - `#SubmissionId,Domain,MissingReviews` — papers that did not get enough reviews (if any)
 * - `#Risk Analysis: 1` followed by critical reviewer IDs (if RiskAnalysis = 1)
 *
 * @param assignment The result of running the assignment algorithm.
 * @param data       The full problem instance (used for missing review checks).
 * @param filename   Path to the output CSV file.
 */
void printAssignment(ReviewAssignment assignment, AssignmentData data, string filename);

#endif