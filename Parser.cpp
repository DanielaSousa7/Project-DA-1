#include "Parser.h"
#include <sstream>
#include <fstream>
using namespace std;

AssignmentData parseCSV(string filename) {
    AssignmentData data;
    string line;
    string section;
    ifstream file(filename);
    while (getline(file,line)) {
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
            getline(ss, token, ','); title = token;
            getline(ss, token, ','); authors = token;
            getline(ss, token, ','); email = token;
            getline(ss, token, ','); primary = stoi(token);
            getline(ss, token, ',');
            if (!token.empty()) secondary = stoi(token);
            data.submissions.push_back({id, title, authors, email, primary, secondary});
        }
        else if (section == "reviewers") {
            int id, primary, secondary;
            string name, email;
            getline(ss, token, ','); id = stoi(token);
            getline(ss, token, ','); name = token;
            getline(ss, token, ','); email = token;
            getline(ss, token, ','); primary = stoi(token);
            getline(ss, token, ',');
            if (!token.empty()) secondary = stoi(token);
            data.reviewers.push_back({id, name, email, primary, secondary});
        }
        else if (section == "parameters") {
            string key, value;
            getline(ss, key, ',');
            getline(ss, value, ',');
            if (key == "MinReviewsPerSubmission") data.parameters.minReviewsPerSubmission = stoi(value);
            else if (key == "MaxReviewsPerReviewer") data.parameters.maxReviewsPerReviewer = stoi(value);
            else if (key == "PrimaryReviewerExpertise") data.parameters.primaryReviewerExpertise = stoi(value);
            else if (key == "SecondaryReviewerExpertise") data.parameters.secondaryReviewerExpertise = stoi(value);
            else if (key == "PrimarySubmissionDomain") data.parameters.primarySubmissionDomain = stoi(value);
            else if (key == "SecondarySubmissionDomain") data.parameters.secondarySubmissionDomain = stoi(value);
        }
        else if (section == "control") {
            string key, value;
            getline(ss, key, ',');
            getline(ss, value, ',');
            if (key == "GenerateAssignments") data.control.generateAssignments = stoi(value);
            else if (key == "RiskAnalysis") data.control.riskAnalysis = stoi(value);
            else if (key == "OutputFileName") data.control.outputFileName = value;
        }
    }
    return data;
}
