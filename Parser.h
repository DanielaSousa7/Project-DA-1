/**
 * @file Parser.h
 * @brief Functions for reading and parsing the input CSV file.
 */

#ifndef PROJECT_DA_1_PARSER_H
#define PROJECT_DA_1_PARSER_H

#include "Data.h"
#include <string>

/**
 * @brief Removes leading and trailing spaces from a string.
 *
 * @param s Input string.
 * @return Trimmed string.
 */
string trim(string s);

/**
 * @brief Removes surrounding double quotes from a string.
 *
 * @param s Input string.
 * @return String without surrounding quotes.
 */
string removeQuotes(string s);

/**
 * @brief Reads a CSV file and stores its data.
 *
 * @param filename Input file name.
 * @return Parsed problem data.
 */
AssignmentData parseCSV(string filename);

#endif