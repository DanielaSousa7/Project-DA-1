#ifndef RISK_ANALYSIS_H
#define RISK_ANALYSIS_H

#include "DataStructures.h"
#include <vector>
#include <string>

using namespace std;

struct RiskResult {
    int K;
    bool anyRisky;
    vector<vector<int>> riskySets;
};

bool assignmentFeasible(const ProblemInstance& instance, int mode);

// K=1: test each reviewer individually (T2.2) — O(R x V x E²)
RiskResult riskAnalysisK1(const ProblemInstance& instance, int mode);

// K>1: test every group of K reviewers (T2.3) — O(C(R,K) x V x E²)
RiskResult riskAnalysisKN(const ProblemInstance& instance, int mode, int K);

// Returns the risk output as lines to be appended to the assignment file.
// Format:
//   #Risk Analysis: K
//   id1, id2, ...    (one line per risky group, or empty if none)
vector<string> riskResultToLines(const RiskResult& result);

#endif