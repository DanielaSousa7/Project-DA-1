#ifndef APPLICATION_H
#define APPLICATION_H

#include "DataStructures.h"
#include "FlowNetwork.h"
#include "RiskAnalysis.h"
#include <string>
#include <optional>

using namespace std;

// Holds the loaded problem instance and exposes the core operations.
// The menu calls these; they don't know or care about the UI.
class Application {
public:
    // Load a .csv file. Throws runtime_error on failure.
    void loadFile(const string& path);

    // True if a file has been successfully loaded.
    bool isLoaded() const;

    // The currently loaded instance (only valid if isLoaded()).
    const ProblemInstance& getInstance() const;

    // Run Max-Flow assignment for the given mode (1, 2 or 3).
    // Returns the result — does NOT write any file.
    FlowResult runAssignment(int mode) const;

    // Write an assignment result to the output file specified in the instance.
    void writeAssignment(const FlowResult& result) const;

    // Run risk analysis for a given K.
    // K=1 → test each reviewer individually     (T2.2)
    // K>1 → test every group of K reviewers     (T2.3)
    RiskResult runRiskAnalysis(int K) const;

    // Write a risk result to "risk_<outputFileName>".
    void writeRisk(const RiskResult& result) const;

    // Batch mode: load file, run assignment + risk analysis as specified
    // in the control section, write output files. Returns exit code.
    static int runBatch(const string& inputFile);

private:
    optional<ProblemInstance> instance_;
    string loadedFile_;
};

#endif
