#include "Application.h"
#include "Parser.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Removes quotes, spaces and carriage returns from a filename
static string cleanFilename(const string& name) {
    string result;
    for (char c : name)
        if (c != '"' && c != '\'' && c != '\r' && c != ' ')
            result += c;
    return result.empty() ? "output.csv" : result;
}

// ─── File loading ─────────────────────────────────────────────────────────────

void Application::loadFile(const string& path) {
    instance_ = parseInputFile(path);
    instance_->control.outputFileName = cleanFilename(instance_->control.outputFileName);
    loadedFile_ = path;
}

bool Application::isLoaded() const {
    return instance_.has_value();
}

const ProblemInstance& Application::getInstance() const {
    if (!instance_) throw runtime_error("No file loaded.");
    return *instance_;
}

// ─── Assignment ───────────────────────────────────────────────────────────────

FlowResult Application::runAssignment(int mode) const {
    return FlowNetwork(getInstance(), mode).getResult();
}

void Application::writeAssignment(const FlowResult& result, const vector<string>& riskLines) const {
    writeAssignmentOutput(result, getInstance().control.outputFileName, riskLines);
}

// ─── Risk Analysis ────────────────────────────────────────────────────────────

RiskResult Application::runRiskAnalysis(int K) const {
    const auto& inst = getInstance();
    int mode = (inst.control.generateAssignments == 0) ? 1
                                                       : inst.control.generateAssignments;
    return (K == 1) ? riskAnalysisK1(inst, mode)
                    : riskAnalysisKN(inst, mode, K);
}

// ─── Batch mode ───────────────────────────────────────────────────────────────

int Application::runBatch(const string& inputFile) {
    try {
        Application app;
        app.loadFile(inputFile);
        const auto& inst = app.getInstance();

        cout << "Loaded: " << inst.submissions.size() << " paper(s), "
             << inst.reviewers.size() << " reviewer(s)\n";

        if (inst.control.generateAssignments != 0) {
            FlowResult result = app.runAssignment(inst.control.generateAssignments);

            // Run risk analysis regardless of success
            vector<string> riskLines;
            if (inst.control.riskAnalysis != 0) {
                RiskResult risk = app.runRiskAnalysis(inst.control.riskAnalysis);
                riskLines = riskResultToLines(risk);
            }

            app.writeAssignment(result, riskLines);
        }

        return 0;
    } catch (const exception& e) {
        cerr << "[Error] " << e.what() << "\n";
        return 1;
    }
}