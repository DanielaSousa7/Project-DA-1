#include "Parser.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

// Removes leading/trailing whitespace and quotes from a string
static string clean(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return s.substr(start, end - start + 1);
}

// Safe stoi — returns 0 if string is empty or not a number
static int safeInt(const string& s) {
    string t = clean(s);
    if (t.empty()) return 0;
    try { return stoi(t); }
    catch (...) { return 0; }
}

AssignmentData parseCSV(string filename) {
    AssignmentData data;
    string line, section;
    ifstream file(filename);
    set<int> submissionIds, reviewerIds;
    vector<string> errors;

    while (getline(file, line)) {
        // Handle Windows line endings
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        if (line[0] == '#') {
            if      (line.find("Submissions") != string::npos) section = "submissions";
            else if (line.find("Reviewers")   != string::npos) section = "reviewers";
            else if (line.find("Parameters")  != string::npos) section = "parameters";
            else if (line.find("Control")     != string::npos) section = "control";
            continue;
        }

        stringstream ss(line);
        string token;

        if (section == "submissions") {
            Submission s;
            getline(ss, token, ','); s.id            = safeInt(token);
            getline(ss, token, ','); s.title          = clean(token);
            getline(ss, token, ','); s.authors        = clean(token);
            getline(ss, token, ','); s.email          = clean(token);
            getline(ss, token, ','); s.primaryDomain  = safeInt(token);
            getline(ss, token, ','); s.secondaryDomain = token.empty() ? -1 : safeInt(token);

            if (s.id <= 0)
                errors.push_back("Invalid submission id: " + to_string(s.id));
            if (submissionIds.count(s.id))
                errors.push_back("Duplicate submission id: " + to_string(s.id));
            submissionIds.insert(s.id);
            data.submissions.push_back(s);
        }
        else if (section == "reviewers") {
            Reviewer r;
            getline(ss, token, ','); r.id                = safeInt(token);
            getline(ss, token, ','); r.name              = clean(token);
            getline(ss, token, ','); r.email             = clean(token);
            getline(ss, token, ','); r.primaryExpertise  = safeInt(token);
            getline(ss, token, ','); r.secondaryExpertise = token.empty() ? -1 : safeInt(token);

            if (r.id <= 0)
                errors.push_back("Invalid reviewer id: " + to_string(r.id));
            if (reviewerIds.count(r.id))
                errors.push_back("Duplicate reviewer id: " + to_string(r.id));
            reviewerIds.insert(r.id);
            data.reviewers.push_back(r);
        }
        else if (section == "parameters") {
            string key, value;
            getline(ss, key, ',');   key   = clean(key);
            getline(ss, value, ','); value = clean(value);
            int v = safeInt(value);
            if      (key == "MinReviewsPerSubmission")    data.parameters.minReviewsPerSubmission    = v;
            else if (key == "MaxReviewsPerReviewer")       data.parameters.maxReviewsPerReviewer       = v;
            else if (key == "PrimaryReviewerExpertise")    data.parameters.primaryReviewerExpertise   = v;
            else if (key == "SecondaryReviewerExpertise")  data.parameters.secondaryReviewerExpertise = v;
            else if (key == "PrimarySubmissionDomain")     data.parameters.primarySubmissionDomain    = v;
            else if (key == "SecondarySubmissionDomain")   data.parameters.secondarySubmissionDomain  = v;
        }
        else if (section == "control") {
            string key, value;
            getline(ss, key, ',');   key   = clean(key);
            getline(ss, value, ','); value = clean(value);
            if      (key == "GenerateAssignments") data.control.generateAssignments = safeInt(value);
            else if (key == "RiskAnalysis")         data.control.riskAnalysis        = safeInt(value);
            else if (key == "OutputFileName")       data.control.outputFileName      = value;
        }
    }

    file.close();
    if (data.control.outputFileName.empty())
        data.control.outputFileName = "output.csv";

    if (!errors.empty()) {
        cerr << "Input file contains errors:\n";
        for (const string& e : errors) cerr << "- " << e << "\n";
    }
    return data;
}
