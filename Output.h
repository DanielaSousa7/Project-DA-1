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
 * @brief Writes the assignment result to a CSV file.
 *
 * @param assignment Final review assignment.
 * @param data Input problem data.
 * @param filename Name of the output file.
 */
void printAssignment(ReviewAssignment assignment, AssignmentData data, string filename);

#endif