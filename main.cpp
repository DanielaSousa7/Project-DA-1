#include "menu.h"
#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <string>
using namespace std;

// Batch mode: ./proj -b input.csv
static int batchMode(const string& inputFile, const string& outputFile = "") {
    AssignmentData data = parseCSV(inputFile);
    cout << "Loaded: " << data.submissions.size() << " submission(s), "
         << data.reviewers.size() << " reviewer(s)\n";

    if (data.control.generateAssignments != 0) {
        if (outputFile != "")
        {
            ReviewAssignment result = runReviewAssignment(data);
            printAssignment(result, data, outputFile);
            cout << "[Output written to: " << outputFile << "]\n";
        }
        else
        {
            ReviewAssignment result = runReviewAssignment(data);
            printAssignment(result, data, data.control.outputFileName);
            cout << "[Output written to: " << data.control.outputFileName << "]\n";
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3 && string(argv[1]) == "-b")
        return batchMode(argv[2]);
    else if (argc == 4 && string(argv[1]) == "-b")
        return batchMode(argv[2], argv[3]);
    return mainMenu();
}