#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// Removes leading/trailing whitespace and quotes from a string
static string clean(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return s.substr(start, end - start + 1);
}

ProblemInstance parseInputFile(const string& filename) {
    ProblemInstance data;
    ifstream file(filename);
    string line, section;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Section headers
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
            getline(ss, token, ','); s.id            = stoi(token);
            getline(ss, token, ','); s.title          = clean(token);
            getline(ss, token, ','); s.authors        = clean(token);
            getline(ss, token, ','); s.email          = clean(token);
            getline(ss, token, ','); s.primaryTopic   = stoi(token);
            getline(ss, token, ','); s.secondaryTopic = token.empty() ? 0 : stoi(token);
            data.submissions.push_back(s);
        }
        else if (section == "reviewers") {
            Reviewer r;
            getline(ss, token, ','); r.id                  = stoi(token);
            getline(ss, token, ','); r.name                = clean(token);
            getline(ss, token, ','); r.email               = clean(token);
            getline(ss, token, ','); r.primaryExpertise    = stoi(token);
            getline(ss, token, ','); r.secondaryExpertise  = token.empty() ? 0 : stoi(token);
            data.reviewers.push_back(r);
        }
        else if (section == "parameters") {
            string key, value;
            getline(ss, key,   ','); key   = clean(key);
            getline(ss, value, ','); value = clean(value);
            int v = stoi(value);
            if      (key == "MinReviewsPerSubmission")    data.params.minReviewsPerSubmission    = v;
            else if (key == "MaxReviewsPerReviewer")       data.params.maxReviewsPerReviewer       = v;
            else if (key == "PrimaryReviewerExpertise")    data.params.primaryReviewerExpertise   = v;
            else if (key == "SecondaryReviewerExpertise")  data.params.secondaryReviewerExpertise = v;
            else if (key == "PrimarySubmissionDomain")     data.params.primarySubmissionDomain    = v;
            else if (key == "SecondarySubmissionDomain")   data.params.secondarySubmissionDomain  = v;
        }
        else if (section == "control") {
            string key, value;
            getline(ss, key,   ','); key   = clean(key);
            getline(ss, value, ','); value = clean(value);
            if      (key == "GenerateAssignments") data.control.generateAssignments = stoi(value);
            else if (key == "RiskAnalysis")         data.control.riskAnalysis        = stoi(value);
            else if (key == "OutputFileName")       data.control.outputFileName      = value;
        }
    }

    if (data.control.outputFileName.empty())
        data.control.outputFileName = "output.csv";

    return data;
}

void printProblemInstance(const ProblemInstance& inst) {
    cout << "=== Submissions (" << inst.submissions.size() << ") ===\n";
    for (const auto& s : inst.submissions)
        cout << "  [" << s.id << "] " << s.title << " | Primary: " << s.primaryTopic << "\n";

    cout << "\n=== Reviewers (" << inst.reviewers.size() << ") ===\n";
    for (const auto& r : inst.reviewers)
        cout << "  [" << r.id << "] " << r.name << " | Primary: " << r.primaryExpertise << "\n";

    cout << "\n=== Control ===\n"
         << "  GenerateAssignments: " << inst.control.generateAssignments << "\n"
         << "  RiskAnalysis:        " << inst.control.riskAnalysis        << "\n"
         << "  OutputFileName:      " << inst.control.outputFileName      << "\n";
}