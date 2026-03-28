/**
 * @file menu.cpp
 * @brief Implementation of the interactive menu.
 */

#include "menu.h"
#include "Parser.h"
#include "Output.h"
#include "Connection.h"
#include <iostream>
#include <limits>
#include <string>
using namespace std;

/**
 * @brief Displays the main menu.
 */
void showMainMenu() {
    cout << "=== Assignment Tool ===\n";
    cout << "1. Read new file\n";
    cout << "2. Browse File Data\n";
    cout << "3. Run assignment\n";
    cout << "0. Exit\n";
    cout << "Select an option: ";
}

/**
 * @brief Displays the data menu.
 */
void showDataMenu() {
    cout << "=== File Data ===\n";
    cout << "1. List Submissions\n";
    cout << "2. List Reviewers\n";
    cout << "3. Show Parameters\n";
    cout << "4. Show Control\n";
    cout << "0. Back\n";
    cout << "Select an option: ";
}

/**
 * @brief Prints all submissions.
 *
 * Time complexity: O(S), where S is the number of submissions.
 *
 * @param data Input problem data.
 */
void printSubmissions(AssignmentData data) {
    cout << "--- Submissions ---\n";
    for (Submission s : data.submissions) {
        cout << "ID: " << s.id;
        cout << ", Title: " << s.title;
        cout << ", Authors: " << s.authors;
        cout << ", Email: " << s.email;
        cout << ", Primary: " << s.primaryDomain;
        cout << ", Secondary: " << s.secondaryDomain << "\n";
    }
}

/**
 * @brief Prints all reviewers.
 *
 * Time complexity: O(R), where R is the number of reviewers.
 *
 * @param data Input problem data.
 */
void printReviewers(AssignmentData data) {
    cout << "--- Reviewers ---\n";
    for (Reviewer r : data.reviewers) {
        cout << "ID: " << r.id;
        cout << ", Name: " << r.name;
        cout << ", Email: " << r.email;
        cout << ", Primary: " << r.primaryExpertise;
        cout << ", Secondary: " << r.secondaryExpertise << "\n";
    }
}

/**
 * @brief Prints the problem parameters.
 *
 * Time complexity: O(1).
 *
 * @param data Input problem data.
 */
void printParameters(AssignmentData data) {
    cout << "--- Parameters ---\n";
    cout << "MinReviewsPerSubmission: " << data.parameters.minReviewsPerSubmission << "\n";
    cout << "MaxReviewsPerReviewer: " << data.parameters.maxReviewsPerReviewer << "\n";
    cout << "PrimaryReviewerExpertise: " << data.parameters.primaryReviewerExpertise << "\n";
    cout << "SecondaryReviewerExpertise: " << data.parameters.secondaryReviewerExpertise << "\n";
    cout << "PrimarySubmissionDomain: " << data.parameters.primarySubmissionDomain << "\n";
    cout << "SecondarySubmissionDomain: " << data.parameters.secondarySubmissionDomain << "\n";
}

/**
 * @brief Prints the control settings.
 *
 * Time complexity: O(1).
 *
 * @param data Input problem data.
 */
void printControl(AssignmentData data) {
    cout << "--- Control ---\n";
    cout << "GenerateAssignments: " << data.control.generateAssignments << "\n";
    cout << "RiskAnalysis: " << data.control.riskAnalysis << "\n";
    cout << "OutputFileName: " << data.control.outputFileName << "\n";
}

/**
 * @brief Runs the data menu.
 *
 * Time complexity: depends on the selected option.
 *
 * @param data Input problem data.
 */
void dataMenu(AssignmentData data) {
    int option = -1;
    while(option != 0) {
        showDataMenu();
        if (cin >> option) {
            cin.ignore();
            switch(option) {
                case 1: cout << "\n"; printSubmissions(data); cout << "\n"; break;
                case 2: cout << "\n"; printReviewers(data); cout << "\n"; break;
                case 3: cout << "\n"; printParameters(data); cout << "\n"; break;
                case 4: cout << "\n"; printControl(data); cout << "\n"; break;
                case 0: break;
                default: cout << "Invalid option!\n\n"; break;
            }
        }
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option!\n\n";
            option = -1;
        }
    }
}

/**
 * @brief Runs the main interactive menu.
 *
 * Time complexity: depends on the selected options. The most expensive
 * operation is running the review assignment algorithm.
 *
 * @return 0 on normal exit.
 */
int mainMenu() {
    string filename;
    cout << "Enter path of your .csv file: ";
    cin >> filename;
    cout << endl;
    AssignmentData data = parseCSV(filename);
    int option = -1;
    while (option != 0) {
        showMainMenu();
        if (cin >> option) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch(option) {
                case 1:
                    cout << "Enter input CSV filename: ";
                    cin >> filename;
                    data = parseCSV(filename);
                    break;
                case 2:
                    cout << '\n';
                    dataMenu(data);
                    break;
                case 3: {
                    cout << "Running assignment...\n";
                    ReviewAssignment result = runReviewAssignment(data);
                    cout << "\n--- Assignments ---\n";
                    for (ReviewAssignmentItem a : result.submissionAssignments) {
                        cout << "Submission " << a.submissionId;
                        cout << " -> Reviewer " << a.reviewerId;
                        cout << " (Domain " << a.matchDomain << ")\n";
                    }
                    cout << "\nTotal: " << result.totalAssignments << "\n";
                    if (data.control.generateAssignments != 0) {
                        printAssignment(result, data, data.control.outputFileName);
                        cout << "Output written to " << data.control.outputFileName << "\n";
                    }
                    break;
                }
                case 0:
                    cout << "Exiting...\n";
                    break;
                default:
                    cout << "Invalid option!\n";
                    break;
            }
        }
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option!\n";
            option = -1;
        }
        cout << "\n";
    }
    return 0;
}