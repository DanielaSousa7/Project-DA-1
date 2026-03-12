#ifndef PARSER_H
#define PARSER_H

#include "DataStructures.h"
#include <string>
#include <stdexcept>

/**
 * @brief Parses a .csv input file into a ProblemInstance.
 *
 * Handles the four sections: #Submissions, #Reviewers, #Parameters, #Control.
 * Lines starting with '#' are treated as section headers or comments.
 * Everything after a '#' character on a data line is ignored.
 * Fields are comma-separated; string fields may be quoted.
 * Validates for duplicate IDs and missing mandatory fields.
 *
 * @param filename Path to the .csv input file.
 * @return A fully populated ProblemInstance.
 * @throws std::runtime_error on any parse or validation error.
 *
 * Time Complexity: O(S + R) where S = number of submissions, R = number of reviewers.
 */
ProblemInstance parseInputFile(const std::string& filename);

/**
 * @brief Prints a human-readable summary of a ProblemInstance to stdout.
 * @param instance The parsed problem instance.
 */
void printProblemInstance(const ProblemInstance& instance);

#endif // PARSER_H
