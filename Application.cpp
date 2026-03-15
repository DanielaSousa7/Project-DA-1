#include "Application.h"
#include "Parser.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// ─── File loading ─────────────────────────────────────────────────────────────

void Application::loadFile(const string& path) {
    instance_ = parseInputFile(path);
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

void Application::writeAssignment(const FlowResult& result) const {
    writeAssignmentOutput(result, getInstance().control.outputFileName);
}

// ─── Risk Analysis ────────────────────────────────────────────────────────────

RiskResult Application::runRiskAnalysis(int K) const {
    const auto& inst = getInstance();
    int mode = (inst.control.generateAssignments == 0) ? 1
                                                       : inst.control.generateAssignments;
    return (K == 1) ? riskAnalysisK1(inst, mode)
                    : riskAnalysisKN(inst, mode, K);
}

void Application::writeRisk(const RiskResult& result) const {
    writeRiskOutput(result, "risk_" + getInstance().control.outputFileName);
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
            int mode = inst.control.generateAssignments;
            FlowResult result = app.runAssignment(mode);
            app.writeAssignment(result);
        }

        if (inst.control.riskAnalysis != 0) {
            RiskResult result = app.runRiskAnalysis(inst.control.riskAnalysis);
            app.writeRisk(result);
        }

        return 0;
    } catch (const exception& e) {
        cerr << "[Error] " << e.what() << "\n";
        return 1;
    }
}
