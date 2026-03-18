#ifndef APPLICATION_H
#define APPLICATION_H

#include "DataStructures.h"
#include "FlowNetwork.h"
#include "RiskAnalysis.h"
#include <string>
#include <optional>
#include <vector>

using namespace std;

class Application {
public:
    void loadFile(const string& path);
    bool isLoaded() const;
    const ProblemInstance& getInstance() const;

    FlowResult runAssignment(int mode) const;

    // Write assignment to file, optionally appending risk lines at the end
    void writeAssignment(const FlowResult& result,
                         const vector<string>& riskLines = {}) const;

    RiskResult runRiskAnalysis(int K) const;

    static int runBatch(const string& inputFile);

private:
    optional<ProblemInstance> instance_;
    string loadedFile_;
};

#endif