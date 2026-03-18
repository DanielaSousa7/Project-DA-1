#include "menu.h"
#include "parser.h"
#include <iostream>
#include <string>
using namespace std;

void showMainMenu() {
    cout << "=== Conference Assignment Tool ===\n";
    cout << "1. Read new file\n";
    cout << "2. Browse Data\n";
    cout << "3. Run assignment\n";
    cout << "4. Run Risk Analysis\n";
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
    cout << "0. Back\n";
    cout << "Select an option: ";
}

void showRiskAnalysisMenu() {
    cout << "=== Conference Assignment Tool ===\n";
    cout << "0. Back\n";
    cout << "Select an option: ";
}

int dataMenu(AssignmentData data) {
    int option;
    do {
        showDataMenu();
        cin >> option;
        cin.ignore(); // limpar \n
        switch(option) {
            case 1: printSubmissions(data); break;
            case 2: printReviewers(data); break;
            case 3: printParameters(data); break;
            case 4: printControl(data); break;
            case 0: break;
            default: cout << "Invalid option!\n"; break;
        }
        cout << "\n";
    } while(option != 0);
    return 0;
}

int assignmentMenu(AssignmentData data) {
    int option;
    do {
        showAssignmentMenu();
        cin >> option;
        cin.ignore(); // limpar \n
        switch(option) {
            case 0: break;
            default: cout << "Invalid option!\n"; break;
        }
        cout << "\n";
    } while(option != 0);
    return 0;
}

int riskAnalysisMenu(AssignmentData data) {
    int option;
    do {
        showRiskAnalysisMenu();
        cin >> option;
        cin.ignore(); // limpar \n
        switch(option) {
            case 0: break;
            default: cout << "Invalid option!\n"; break;
        }
        cout << "\n";
    } while(option != 0);
    return 0;
}

void printSubmissions(const AssignmentData &data) {
    cout << "--- Submissions ---\n";
    for (const auto &s : data.submissions) {
        cout << "ID: " << s.id 
             << ", Title: " << s.title 
             << ", Authors: " << s.authors
             << ", Email: " << s.email 
             << ", Primary: " << s.primaryDomain 
             << ", Secondary: " << s.secondaryDomain << "\n";
    }
}

void printReviewers(const AssignmentData &data) {
    cout << "--- Reviewers ---\n";
    for (const auto &r : data.reviewers) {
        cout << "ID: " << r.id 
             << ", Name: " << r.name 
             << ", Email: " << r.email
             << ", Primary: " << r.primaryExpertise 
             << ", Secondary: " << r.secondaryExpertise << "\n";
    }
}

void printParameters(const AssignmentData &data) {
    cout << "--- Parameters ---\n";
    cout << "MinReviewsPerSubmission: " << data.parameters.minReviewsPerSubmission << "\n";
    cout << "MaxReviewsPerReviewer: " << data.parameters.maxReviewsPerReviewer << "\n";
    cout << "PrimaryReviewerExpertise: " << data.parameters.primaryReviewerExpertise << "\n";
    cout << "SecondaryReviewerExpertise: " << data.parameters.secondaryReviewerExpertise << "\n";
    cout << "PrimarySubmissionDomain: " << data.parameters.primarySubmissionDomain << "\n";
    cout << "SecondarySubmissionDomain: " << data.parameters.secondarySubmissionDomain << "\n";
}

void printControl(const AssignmentData &data) {
    cout << "--- Control ---\n";
    cout << "GenerateAssignments: " << data.control.generateAssignments << "\n";
    cout << "RiskAnalysis: " << data.control.riskAnalysis << "\n";
    cout << "OutputFileName: " << data.control.outputFileName << "\n";
}

int mainMenu() {
    // TODO: aqui vem todo o código que estava dentro do main do menu
    string filename;
    cout << "Enter input CSV filename: ";
    cin >> filename;

    AssignmentData data = parseCSV(filename);

    int option = -1;
    do {
        showMainMenu();
        cin >> option;
        cin.ignore(); // limpar \n
        switch(option) {
            case 1:
                cout << "Enter input CSV filename: ";
                cin >> filename;
                data = parseCSV(filename);
                break;
            case 2: cout << '\n' ; dataMenu(data); break;
            case 3: cout << '\n' ; assignmentMenu(data); break;
            case 4: cout << '\n' ; riskAnalysisMenu(data); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid option!\n"; break;
        }
        cout << "\n";
    } while(option > 0);
    return 0;
}