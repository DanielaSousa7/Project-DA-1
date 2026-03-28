/**
 * @file Batch.cpp
 * @brief Implementation of the batch mode.
 */

#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <string>
#include "Batch.h"
using namespace std;

/**
 * @brief Runs the program in batch mode.
 *
 * Time complexity: depends mainly on parseCSV, runReviewAssignment
 * and printAssignment.
 *
 * @param inputFile Input file name.
 * @param outputFile Output file name.
 */

void batchMode(string inputFile, string outputFile) {
    AssignmentData data = parseCSV(inputFile);
    if (data.hasErrors) {
        cerr << "Errors found in input file. Aborting batch.\n";
        return;
    }
    cout << "Loaded: " << data.submissions.size() << " submission(s), " << data.reviewers.size() << " reviewer(s)\n";
    if (data.control.generateAssignments != 0) {
        ReviewAssignment result = runReviewAssignment(data);
        printAssignment(result, data, outputFile);
        cout << "[Output written to: " << outputFile << "]\n";
    }
}