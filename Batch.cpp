/**
* @file Batch.cpp
 * @brief Implementation of the batch mode.
 *
 */

#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <string>
#include "Batch.h"
using namespace std;

/**
 * @brief Runs the tool in batch mode, reading from a CSV file and writing output.
 *
 * If an explicit output filename is provided as a third argument, it overrides
 * the filename specified in the CSV control section.
 *
 * @param inputFile  Path to the input CSV file.
 * @param outputFile Optional output filename override (empty = use CSV setting).
 */

void batchMode(string inputFile, string outputFile) {
    AssignmentData data = parseCSV(inputFile);
    cout << "Loaded: " << data.submissions.size() << " submission(s), " << data.reviewers.size() << " reviewer(s)\n";
    if (data.control.generateAssignments != 0) {
        ReviewAssignment result = runReviewAssignment(data);
        printAssignment(result, data, outputFile);
        cout << "[Output written to: " << outputFile << "]\n";
    }
}