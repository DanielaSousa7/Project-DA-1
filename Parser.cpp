/**
 * @file Parser.cpp
 * @brief Implementation of the CSV parser.
 */

#include "Parser.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

/**
 * @brief Removes leading and trailing spaces from a string.
 *
 * Time complexity: O(n), where n is the length of the string.
 *
 * @param s Input string.
 * @return Trimmed string.
 */
string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

/**
 * @brief Removes surrounding double quotes from a string.
 *
 * Time complexity: O(n), where n is the length of the string.
 *
 * @param s Input string.
 * @return String without surrounding quotes.
 */
string removeQuotes(string s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

/**
 * @brief Reads a CSV file and stores its data.
 *
 * Errors (invalid IDs, duplicates) are collected and printed to stderr at the end.
 * Parsing continues even when errors are found.
 *
 * Time complexity: O(L log L), where L is the number of lines in the file.
 *
 * @param filename Input file name.
 * @return Parsed problem data.
 */
AssignmentData parseCSV(string filename) {
    AssignmentData data;
    string line;
    string section;
    ifstream file(filename);
    set<int> submissionIds;
    set<int> reviewerIds;
    vector<string> errors;
    while (getline(file,line)) {
        if (line.empty()) continue;
        if (line[0] == '#') {
            if (line.find("Submissions") != string::npos) section = "submissions";
            else if (line.find("Reviewers") != string::npos) section = "reviewers";
            else if (line.find("Parameters") != string::npos) section = "parameters";
            else if (line.find("Control") != string::npos) section = "control";
            continue;
        }
        stringstream ss(line);
        string token;
        if (section == "submissions") {
            int id, primary, secondary;
            string title, authors, email;
            getline(ss, token, ','); id = stoi(token);
            if (id <= 0)
                errors.push_back("Invalid submission id: " + to_string(id));
            if (submissionIds.count(id))
                errors.push_back("Duplicate submission id: " + to_string(id));
            submissionIds.insert(id);
            getline(ss, token, ','); title = token;
            getline(ss, token, ','); authors = token;
            getline(ss, token, ','); email = token;
            getline(ss, token, ','); primary = stoi(token);
            getline(ss, token, ',');
            if (token.empty()) secondary = -1;
            else secondary = atoi(token.c_str());
            data.submissions.push_back({id, title, authors, email, primary, secondary});
        }
        else if (section == "reviewers") {
            int id, primary, secondary;
            string name, email;
            getline(ss, token, ','); id = stoi(token);
            if (id <= 0)
                errors.push_back("Invalid reviewer id: " + to_string(id));
            if (reviewerIds.count(id))
                errors.push_back("Duplicate reviewer id: " + to_string(id));
            reviewerIds.insert(id);
            getline(ss, token, ','); name = token;
            getline(ss, token, ','); email = token;
            getline(ss, token, ','); primary = stoi(token);
            getline(ss, token, ',');
            if (token.empty()) secondary = -1;
            else secondary = atoi(token.c_str());
            data.reviewers.push_back({id, name, email, primary, secondary});
        }
        else if (section == "parameters") {
            string key, value;
            getline(ss, key, ',');
            getline(ss, value, ',');
            int v = stoi(value);
            if (v < 0)
                errors.push_back("Negative parameter value: " + key);
            if (key == "MinReviewsPerSubmission") data.parameters.minReviewsPerSubmission = v;
            else if (key == "MaxReviewsPerReviewer") data.parameters.maxReviewsPerReviewer = v;
            else if (key == "PrimaryReviewerExpertise") data.parameters.primaryReviewerExpertise = v;
            else if (key == "SecondaryReviewerExpertise") data.parameters.secondaryReviewerExpertise = v;
            else if (key == "PrimarySubmissionDomain") data.parameters.primarySubmissionDomain = v;
            else if (key == "SecondarySubmissionDomain") data.parameters.secondarySubmissionDomain = v;
        }
        else if (section == "control") {
            string key, value;
            getline(ss, key, ',');
            getline(ss, value, ',');
            key = trim(key);
            value = trim(value);
            if (key == "GenerateAssignments") {
                int v = stoi(value);
                if (v < 0 || v > 3)
                    errors.push_back("Invalid GenerateAssignments value");
                data.control.generateAssignments = v;
            }
            else if (key == "RiskAnalysis") {
                int v = stoi(value);
                if (v < 0)
                    errors.push_back("Invalid RiskAnalysis value");
                data.control.riskAnalysis = v;
            }
            else if (key == "OutputFileName") data.control.outputFileName = removeQuotes(value);
        }
    }
    file.close();
    if (data.control.generateAssignments != 0 && data.control.outputFileName.empty()) {
        data.control.outputFileName = "output.csv";
    }
    if (!errors.empty()) {
        data.hasErrors = true;
        cerr << "Input file contains errors:\n";
        for (string e : errors)
            cerr << "- " << e << endl;
    }
    return data;
}