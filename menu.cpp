#include "menu.h"
#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <limits>
#include <string>
using namespace std;

void showMainMenu() {
    cout << "=== Conference Assignment Tool ===\n";
    cout << "1. Read new file\n";
    cout << "2. Browse Data\n";
    cout << "3. Run assignment\n";
    cout << "0. Exit\n";
    cout << "Select an option: ";
}

void showDataMenu() {
    cout << "=== Conference Assignment Tool ===\n";
    cout << "1. List Submissions\n";
    cout << "2. List Reviewers\n";
    cout << "3. Show Parameters\n";
    cout << "4. Show Control\n";
    cout << "0. Back\n";
    cout << "Select an option: ";
}

void showAssignmentMenu() {
    cout << "=== Conference Assignment Tool ===\n";
    cout << "1. Run assignment (mode from file)\n";
    cout << "2. Run assignment mode 1 (primary only)\n";
    cout << "3. Run assignment mode 2 (sub primary+secondary / rev primary)\n";
    cout << "4. Run assignment mode 3 (all combinations)\n";
    cout << "0. Back\n";
    cout << "Select an option: ";
}

void printSubmissions(AssignmentData data) {
    cout << "--- Submissions ---\n";
    for (const Submission& s : data.submissions) {
        cout << "ID: " << s.id << ", Title: " << s.title
             << ", Primary: " << s.primaryDomain
             << ", Secondary: " << s.secondaryDomain << "\n";
    }
}

void printReviewers(AssignmentData data) {
    cout << "--- Reviewers ---\n";
    for (const Reviewer& r : data.reviewers) {
        cout << "ID: " << r.id << ", Name: " << r.name
             << ", Primary: " << r.primaryExpertise
             << ", Secondary: " << r.secondaryExpertise << "\n";
    }
}

void printParameters(AssignmentData data) {
    cout << "--- Parameters ---\n";
    cout << "MinReviewsPerSubmission:    " << data.parameters.minReviewsPerSubmission    << "\n";
    cout << "MaxReviewsPerReviewer:      " << data.parameters.maxReviewsPerReviewer      << "\n";
    cout << "PrimaryReviewerExpertise:   " << data.parameters.primaryReviewerExpertise   << "\n";
    cout << "SecondaryReviewerExpertise: " << data.parameters.secondaryReviewerExpertise << "\n";
    cout << "PrimarySubmissionDomain:    " << data.parameters.primarySubmissionDomain    << "\n";
    cout << "SecondarySubmissionDomain:  " << data.parameters.secondarySubmissionDomain  << "\n";
}

void printControl(AssignmentData data) {
    cout << "--- Control ---\n";
    cout << "GenerateAssignments: " << data.control.generateAssignments << "\n";
    cout << "RiskAnalysis:        " << data.control.riskAnalysis        << "\n";
    cout << "OutputFileName:      " << data.control.outputFileName      << "\n";
}

void dataMenu(AssignmentData data) {
    int option = -1;
    while (option != 0) {
        showDataMenu();
        if (cin >> option) {
            cin.ignore();
            switch (option) {
                case 1: cout << "\n"; printSubmissions(data); cout << "\n"; break;
                case 2: cout << "\n"; printReviewers(data);   cout << "\n"; break;
                case 3: cout << "\n"; printParameters(data);  cout << "\n"; break;
                case 4: cout << "\n"; printControl(data);     cout << "\n"; break;
                case 0: break;
                default: cout << "Invalid option!\n\n"; break;
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option!\n\n";
            option = -1;
        }
    }
}

static void runAndPrint(AssignmentData data, int modeOverride = -1) {
    if (modeOverride != -1) data.control.generateAssignments = modeOverride;

    ReviewAssignment result = runReviewAssignment(data);

    cout << "\n--- Assignments ---\n";
    for (const ReviewAssignmentItem& a : result.submissionAssignments)
        cout << "Submission " << a.submissionId << " -> Reviewer " << a.reviewerId
             << " (Domain " << a.matchDomain << ")\n";
    cout << "Total: " << result.totalAssignments << "\n";

    if (data.control.generateAssignments != 0) {
        printAssignment(result, data, data.control.outputFileName);
        cout << "Output written to " << data.control.outputFileName << "\n";
    }
}

void assignmentMenu(AssignmentData data) {
    int option = -1;
    while (option != 0) {
        showAssignmentMenu();
        if (cin >> option) {
            cin.ignore();
            switch (option) {
                case 1: cout << "\n"; runAndPrint(data);    cout << "\n"; break;
                case 2: cout << "\n"; runAndPrint(data, 1); cout << "\n"; break;
                case 3: cout << "\n"; runAndPrint(data, 2); cout << "\n"; break;
                case 4: cout << "\n"; runAndPrint(data, 3); cout << "\n"; break;
                case 0: break;
                default: cout << "Invalid option!\n\n"; break;
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option!\n\n";
            option = -1;
        }
    }
}

int mainMenu() {
    string filename;
    cout << "Enter input CSV filename: ";
    cin >> filename;
    AssignmentData data = parseCSV(filename);
    int option = -1;
    while (option != 0) {
        showMainMenu();
        if (cin >> option) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (option) {
                case 1:
                    cout << "Enter input CSV filename: ";
                    cin >> filename;
                    data = parseCSV(filename);
                    break;
                case 2: cout << "\n"; dataMenu(data);       cout << "\n"; break;
                case 3: cout << "\n"; assignmentMenu(data); cout << "\n"; break;
                case 0: cout << "Exiting...\n"; break;
                default: cout << "Invalid option!\n\n"; break;
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option!\n\n";
            option = -1;
        }
        if (option != 0) cout << "\n";
    }
    return 0;
}
