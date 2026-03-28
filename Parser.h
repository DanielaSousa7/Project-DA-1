/**
* @file Parser.h
 * @brief Functions for reading and parsing the input CSV file.
 */

#ifndef PROJECT_DA_1_PARSER_H
#define PROJECT_DA_1_PARSER_H

#include "Data.h"
#include <string>

/**
 * @brief Removes leading and trailing whitespace from a string.
 * @param s The input string.
 * @return The trimmed string.
 */
string trim(string s);

/**
 * @brief Removes surrounding double-quotes from a string if present.
 * @param s The input string.
 * @return The unquoted string.
 */
string removeQuotes(string s);

/**
 * @brief Parses a conference assignment CSV file into an AssignmentData struct.
 *
 * Reads the four sections of the file (#Submissions, #Reviewers, #Parameters, #Control).
 * Reports any errors (duplicate IDs, invalid values) to stderr but continues parsing.
 *
 * @param filename Path to the .csv input file.
 * @return A fully populated AssignmentData struct.
 */
AssignmentData parseCSV(string filename);

#endif