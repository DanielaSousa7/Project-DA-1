#include "Parser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>
#include <stdexcept>

using namespace std;

// ─── Internal helpers ────────────────────────────────────────────────────────

/**
 * @brief Strips leading and trailing whitespace from a string.
 */
static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

/**
 * @brief Removes surrounding double-quotes from a string if present.
 */
static string unquote(const string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

/**
 * @brief Strips everything from the first '#' onward in a line (inline comments).
 *        Returns the cleaned line.
 */
static string stripComment(const string& line) {
    // We need to be careful: '#' inside quoted strings should NOT be stripped.
    bool inQuote = false;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '"') inQuote = !inQuote;
        if (line[i] == '#' && !inQuote) return line.substr(0, i);
    }
    return line;
}

/**
 * @brief Splits a CSV line into tokens, respecting quoted fields.
 *        Handles fields like: 31, "Title, with comma", author, ...
 */
static vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    string current;
    bool inQuote = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuote = !inQuote;
            current += c;
        } else if (c == ',' && !inQuote) {
            tokens.push_back(trim(unquote(trim(current))));
            current.clear();
        } else {
            current += c;
        }
    }
    tokens.push_back(trim(unquote(trim(current))));
    return tokens;
}

/**
 * @brief Safely converts a string to int. Returns 0 if the string is empty.
 * @throws runtime_error if the string is non-empty but not a valid integer.
 */
static int safeStoi(const string& s, const string& context) {
    string t = trim(s);
    if (t.empty()) return 0;
    try {
        size_t pos;
        int val = stoi(t, &pos);
        if (pos != t.size()) throw invalid_argument("trailing chars");
        return val;
    } catch (...) {
        throw runtime_error("Invalid integer value '" + t + "' in " + context);
    }
}

// ─── Section parsers ─────────────────────────────────────────────────────────

/**
 * @brief Parses one submission line.
 * Expected fields: Id, Title, Authors, E-mail, Primary[, Secondary]
 */
static Submission parseSubmissionLine(const string& line, int lineNo) {
    auto fields = splitCSV(line);
    if (fields.size() < 5)
        throw runtime_error("Line " + to_string(lineNo) +
                                 ": Submission needs at least 5 fields (Id,Title,Authors,Email,Primary)");

    Submission s;
    s.id              = safeStoi(fields[0], "submission ID at line " + to_string(lineNo));
    s.title           = fields[1];
    s.authors         = fields[2];
    s.email           = fields[3];
    s.primaryTopic    = safeStoi(fields[4], "primary topic at line " + to_string(lineNo));
    s.secondaryTopic  = (fields.size() > 5) ? safeStoi(fields[5], "secondary topic at line " + to_string(lineNo)) : 0;

    if (s.id <= 0)
        throw runtime_error("Line " + to_string(lineNo) + ": Submission ID must be positive");
    if (s.primaryTopic <= 0)
        throw runtime_error("Line " + to_string(lineNo) + ": Primary topic must be a positive integer");

    return s;
}

/**
 * @brief Parses one reviewer line.
 * Expected fields: Id, Name, E-mail, Primary[, Secondary]
 */
static Reviewer parseReviewerLine(const string& line, int lineNo) {
    auto fields = splitCSV(line);
    if (fields.size() < 4)
        throw runtime_error("Line " + to_string(lineNo) +
                                 ": Reviewer needs at least 4 fields (Id,Name,Email,Primary)");

    Reviewer r;
    r.id                  = safeStoi(fields[0], "reviewer ID at line " + to_string(lineNo));
    r.name                = fields[1];
    r.email               = fields[2];
    r.primaryExpertise    = safeStoi(fields[3], "primary expertise at line " + to_string(lineNo));
    r.secondaryExpertise  = (fields.size() > 4) ? safeStoi(fields[4], "secondary expertise at line " + to_string(lineNo)) : 0;

    if (r.id <= 0)
        throw runtime_error("Line " + to_string(lineNo) + ": Reviewer ID must be positive");
    if (r.primaryExpertise <= 0)
        throw runtime_error("Line " + to_string(lineNo) + ": Primary expertise must be a positive integer");

    return r;
}

/**
 * @brief Parses a key-value line (used in #Parameters and #Control sections).
 * Returns {key, value} after trimming.
 */
static pair<string, string> parseKeyValue(const string& line, int lineNo) {
    size_t comma = line.find(',');
    if (comma == string::npos)
        throw runtime_error("Line " + to_string(lineNo) + ": Expected 'Key, Value' format");
    return { trim(line.substr(0, comma)), trim(unquote(trim(line.substr(comma + 1)))) };
}

// ─── Main parser ─────────────────────────────────────────────────────────────

ProblemInstance parseInputFile(const string& filename) {
    // Validate extension
    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".csv")
        throw runtime_error("Input file must have a .csv extension: " + filename);

    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Cannot open file: " + filename);

    ProblemInstance instance;
    vector<string> errors;  // Collect all errors before throwing

    // Possible parser states
    enum class Section { NONE, SUBMISSIONS, REVIEWERS, PARAMETERS, CONTROL };
    Section currentSection = Section::NONE;

    string rawLine;
    int lineNo = 0;

    while (getline(file, rawLine)) {
        ++lineNo;

        // ── Section header detection ──────────────────────────────────────────
        // MUST use the raw line here — stripComment would erase '#Submissions' etc.
        string rawTrimmed = trim(rawLine);
        if (!rawTrimmed.empty() && rawTrimmed[0] == '#') {
            // Convert to uppercase for case-insensitive matching
            string upper = rawTrimmed;
            transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

            if (upper.find("#SUBMISSIONS") != string::npos)
                currentSection = Section::SUBMISSIONS;
            else if (upper.find("#REVIEWERS") != string::npos)
                currentSection = Section::REVIEWERS;
            else if (upper.find("#PARAMETERS") != string::npos)
                currentSection = Section::PARAMETERS;
            else if (upper.find("#CONTROL") != string::npos)
                currentSection = Section::CONTROL;
            // else: lone '#' or column-header comment line — just skip
            continue;
        }

        // Strip inline comments and skip blank data lines
        string line = trim(stripComment(rawLine));
        if (line.empty()) continue;

        // ── Data lines ────────────────────────────────────────────────────────
        try {
            switch (currentSection) {
                case Section::SUBMISSIONS: {
                    Submission s = parseSubmissionLine(line, lineNo);
                    instance.submissions.push_back(s);
                    break;
                }
                case Section::REVIEWERS: {
                    Reviewer r = parseReviewerLine(line, lineNo);
                    instance.reviewers.push_back(r);
                    break;
                }
                case Section::PARAMETERS: {
                    auto [key, value] = parseKeyValue(line, lineNo);
                    string keyUpper = key;
                    transform(keyUpper.begin(), keyUpper.end(), keyUpper.begin(), ::toupper);

                    if      (keyUpper == "MINREVIEWSPERSUBMISSION")    instance.params.minReviewsPerSubmission    = safeStoi(value, key);
                    else if (keyUpper == "MAXREVIEWSPERREVIEWER")       instance.params.maxReviewsPerReviewer       = safeStoi(value, key);
                    else if (keyUpper == "PRIMARYREVIEWEREXPERTISE")    instance.params.primaryReviewerExpertise   = safeStoi(value, key);
                    else if (keyUpper == "SECONDARYREVIEWEREXPERTISE")  instance.params.secondaryReviewerExpertise = safeStoi(value, key);
                    else if (keyUpper == "PRIMARYSUBMISSIONDOMAIN")     instance.params.primarySubmissionDomain    = safeStoi(value, key);
                    else if (keyUpper == "SECONDARYSUBMISSIONDOMAIN")   instance.params.secondarySubmissionDomain  = safeStoi(value, key);
                    else
                        errors.push_back("Line " + to_string(lineNo) + ": Unknown parameter '" + key + "'");
                    break;
                }
                case Section::CONTROL: {
                    auto [key, value] = parseKeyValue(line, lineNo);
                    string keyUpper = key;
                    transform(keyUpper.begin(), keyUpper.end(), keyUpper.begin(), ::toupper);

                    if      (keyUpper == "GENERATEASSIGNMENTS") instance.control.generateAssignments = safeStoi(value, key);
                    else if (keyUpper == "RISKANALYSIS")        instance.control.riskAnalysis        = safeStoi(value, key);
                    else if (keyUpper == "OUTPUTFILENAME")      instance.control.outputFileName      = value.empty() ? "output.csv" : value;
                    else
                        errors.push_back("Line " + to_string(lineNo) + ": Unknown control key '" + key + "'");
                    break;
                }
                case Section::NONE:
                    errors.push_back("Line " + to_string(lineNo) + ": Data found before any section header");
                    break;
            }
        } catch (const runtime_error& e) {
            errors.push_back(e.what());
        }
    }

    // ── Post-parse validation ─────────────────────────────────────────────────

    // Check for duplicate submission IDs
    set<int> subIds;
    for (const auto& s : instance.submissions) {
        if (!subIds.insert(s.id).second)
            errors.push_back("Duplicate submission ID: " + to_string(s.id));
    }

    // Check for duplicate reviewer IDs
    set<int> revIds;
    for (const auto& r : instance.reviewers) {
        if (!revIds.insert(r.id).second)
            errors.push_back("Duplicate reviewer ID: " + to_string(r.id));
    }

    // Check mandatory parameters
    if (instance.params.minReviewsPerSubmission <= 0)
        errors.push_back("MinReviewsPerSubmission must be > 0");
    if (instance.params.maxReviewsPerReviewer <= 0)
        errors.push_back("MaxReviewsPerReviewer must be > 0");

    // Check control values
    if (instance.control.generateAssignments < 0 || instance.control.generateAssignments > 3)
        errors.push_back("GenerateAssignments must be 0, 1, 2, or 3");
    if (instance.control.riskAnalysis < 0)
        errors.push_back("RiskAnalysis must be >= 0");

    // If any errors, report them all at once
    if (!errors.empty()) {
        string msg = "Input file errors:\n";
        for (const auto& e : errors) msg += "  - " + e + "\n";
        throw runtime_error(msg);
    }

    return instance;
}

// ─── Display helper ───────────────────────────────────────────────────────────

void printProblemInstance(const ProblemInstance& instance) {
    cout << "=== Submissions (" << instance.submissions.size() << ") ===\n";
    for (const auto& s : instance.submissions) {
        cout << "  [" << s.id << "] \"" << s.title << "\""
                  << " | " << s.authors
                  << " | Primary: " << s.primaryTopic;
        if (s.secondaryTopic) cout << " | Secondary: " << s.secondaryTopic;
        cout << "\n";
    }

    cout << "\n=== Reviewers (" << instance.reviewers.size() << ") ===\n";
    for (const auto& r : instance.reviewers) {
        cout << "  [" << r.id << "] " << r.name
                  << " | Primary: " << r.primaryExpertise;
        if (r.secondaryExpertise) cout << " | Secondary: " << r.secondaryExpertise;
        cout << "\n";
    }

    cout << "\n=== Parameters ===\n";
    cout << "  MinReviewsPerSubmission:    " << instance.params.minReviewsPerSubmission   << "\n";
    cout << "  MaxReviewsPerReviewer:      " << instance.params.maxReviewsPerReviewer      << "\n";
    cout << "  PrimaryReviewerExpertise:   " << instance.params.primaryReviewerExpertise   << "\n";
    cout << "  SecondaryReviewerExpertise: " << instance.params.secondaryReviewerExpertise << "\n";
    cout << "  PrimarySubmissionDomain:    " << instance.params.primarySubmissionDomain    << "\n";
    cout << "  SecondarySubmissionDomain:  " << instance.params.secondarySubmissionDomain  << "\n";

    cout << "\n=== Control ===\n";
    cout << "  GenerateAssignments: " << instance.control.generateAssignments << "\n";
    cout << "  RiskAnalysis:        " << instance.control.riskAnalysis        << "\n";
    cout << "  OutputFileName:      " << instance.control.outputFileName      << "\n";
}
