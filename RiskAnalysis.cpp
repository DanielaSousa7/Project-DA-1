#include "RiskAnalysis.h"
#include "FlowNetwork.h"
#include <algorithm>
#include <iostream>

using namespace std;

bool assignmentFeasible(const ProblemInstance& instance, int mode) {
    if (instance.submissions.empty() || instance.reviewers.empty()) return false;
    return FlowNetwork(instance, mode).getResult().success;
}

// K=1: remove each reviewer and check feasibility
RiskResult riskAnalysisK1(const ProblemInstance& instance, int mode) {
    RiskResult result;
    result.K = 1;
    result.anyRisky = false;

    // If the base assignment is already infeasible, no reviewer is "risky"
    // (it was already broken — removing one more can't cause a "new" failure)
    if (!assignmentFeasible(instance, mode))
        return result;

    for (int j = 0; j < (int)instance.reviewers.size(); ++j) {
        ProblemInstance reduced = instance;
        reduced.reviewers.erase(reduced.reviewers.begin() + j);

        if (!assignmentFeasible(reduced, mode)) {
            result.riskySets.push_back({ instance.reviewers[j].id });
            result.anyRisky = true;
        }
    }

    sort(result.riskySets.begin(), result.riskySets.end(),
         [](const auto& a, const auto& b){ return a[0] < b[0]; });

    return result;
}

// Generate all C(n,k) combinations of indices
static vector<vector<int>> combinations(int n, int k) {
    vector<vector<int>> result;
    if (k > n || k <= 0) return result;

    vector<int> sel(n, 0);
    fill(sel.begin(), sel.begin() + k, 1);
    sort(sel.begin(), sel.end(), greater<int>());

    do {
        vector<int> combo;
        for (int i = 0; i < n; ++i)
            if (sel[i]) combo.push_back(i);
        result.push_back(combo);
    } while (prev_permutation(sel.begin(), sel.end()));

    return result;
}

// K>1: remove every group of K reviewers and check feasibility
RiskResult riskAnalysisKN(const ProblemInstance& instance, int mode, int K) {
    RiskResult result;
    result.K = K;
    result.anyRisky = false;

    int R = (int)instance.reviewers.size();
    if (K >= R) {
        vector<int> all;
        for (const auto& r : instance.reviewers) all.push_back(r.id);
        result.riskySets.push_back(all);
        result.anyRisky = true;
        return result;
    }

    auto combos = combinations(R, K);
    if (combos.size() > 10000)
        cerr << "[Warning] Testing C(" << R << "," << K << ") = " << combos.size() << " groups.\n";

    for (const auto& indices : combos) {
        ProblemInstance reduced = instance;
        vector<int> sorted = indices;
        sort(sorted.rbegin(), sorted.rend());
        for (int idx : sorted)
            reduced.reviewers.erase(reduced.reviewers.begin() + idx);

        if (!assignmentFeasible(reduced, mode)) {
            vector<int> ids;
            for (int idx : indices) ids.push_back(instance.reviewers[idx].id);
            sort(ids.begin(), ids.end());
            result.riskySets.push_back(ids);
            result.anyRisky = true;
        }
    }

    sort(result.riskySets.begin(), result.riskySets.end());
    return result;
}

// Convert risk result to lines for appending to the output file
vector<string> riskResultToLines(const RiskResult& result) {
    vector<string> lines;
    lines.push_back("#Risk Analysis: " + to_string(result.K));

    if (result.K == 1) {
        // For K=1, all critical reviewer IDs go on a SINGLE line: "7, 8"
        if (result.anyRisky) {
            string line;
            for (int i = 0; i < (int)result.riskySets.size(); ++i) {
                if (i) line += ", ";
                line += to_string(result.riskySets[i][0]);
            }
            lines.push_back(line);
        } else {
            // No risky reviewers: add empty line after header
            lines.push_back("");
        }
    } else {
        // For K>1, each risky group goes on its own line: "1, 2"
        for (const auto& group : result.riskySets) {
            string line;
            for (int i = 0; i < (int)group.size(); ++i) {
                if (i) line += ", ";
                line += to_string(group[i]);
            }
            lines.push_back(line);
        }
        if (!result.anyRisky) lines.push_back("");
    }

    return lines;
}