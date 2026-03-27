#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <string>
#include "Batch.h"
using namespace std;

void batchMode(string inputFile, string outputFile) {
    AssignmentData data = parseCSV(inputFile);
    cout << "Loaded: " << data.submissions.size() << " submission(s), " << data.reviewers.size() << " reviewer(s)\n";
    if (data.control.generateAssignments != 0) {
        ReviewAssignment result = runReviewAssignment(data);
        printAssignment(result, data, outputFile);
        cout << "[Output written to: " << outputFile << "]\n";
    }
}