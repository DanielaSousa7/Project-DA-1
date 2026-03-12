#include "RiskAnalysis.h"
#include "FlowNetwork.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>

// ─── Feasibility check ────────────────────────────────────────────────────────

/**
 * Builds a FlowNetwork for the given instance and mode and returns whether
 * the resulting max-flow satisfies all minimum review requirements.
 * This is the core subroutine shared by all risk analysis variants.
 *
 * Time complexity: O(V * E²) — one Edmonds-Karp run.
 */
bool assignmentFeasible(const ProblemInstance& instance, int mode) {
    if (instance.submissions.empty() || instance.reviewers.empty())
        return false;
    FlowNetwork fn(instance, mode);
    return fn.getResult().success;
}

// ─── K = 1 ────────────────────────────────────────────────────────────────────

/**
 * For each reviewer j, build a reduced instance without reviewer j and check
 * feasibility. Collect all IDs whose removal breaks the assignment.
 *
 * Time complexity: O(R * V * E²)
 */
RiskResult riskAnalysisK1(const ProblemInstance& instance, int mode) {
    RiskResult result;
    result.K = 1;
    result.anyRisky = false;

    for (size_t j = 0; j < instance.reviewers.size(); ++j) {
        ProblemInstance reduced = instance;
        reduced.reviewers.erase(reduced.reviewers.begin() + (int)j);

        if (!assignmentFeasible(reduced, mode)) {
            result.riskySets.push_back({ instance.reviewers[j].id });
            result.anyRisky = true;
        }
    }

    // Sort risky sets by reviewer ID for deterministic output
    std::sort(result.riskySets.begin(), result.riskySets.end(),
              [](const auto& a, const auto& b){ return a[0] < b[0]; });

    return result;
}

// ─── K > 1 ────────────────────────────────────────────────────────────────────

/**
 * Generates all C(n, k) combinations of indices from [0, n).
 * Each combination is a vector of k indices in ascending order.
 *
 * Time complexity: O(C(n,k) * k)
 */
static std::vector<std::vector<int>> combinations(int n, int k) {
    std::vector<std::vector<int>> result;
    if (k > n || k <= 0) return result;

    // Use bitmask / selector vector approach
    std::vector<int> selector(n, 0);
    std::fill(selector.begin(), selector.begin() + k, 1);
    std::sort(selector.begin(), selector.end(), std::greater<int>());

    do {
        std::vector<int> combo;
        for (int i = 0; i < n; ++i)
            if (selector[i]) combo.push_back(i);
        result.push_back(combo);
    } while (std::prev_permutation(selector.begin(), selector.end()));

    return result;
}

/**
 * Enumerates all C(R, K) subsets of K reviewers.
 * For each subset, builds a reduced instance without those reviewers and
 * checks feasibility. Reports all subsets whose removal breaks the assignment.
 *
 * Time complexity: O(C(R,K) * V * E²)
 *
 * Practical limits:
 *   K=1: R runs
 *   K=2: R*(R-1)/2 runs
 *   K=3: R*(R-1)*(R-2)/6 runs
 * Warns the user if C(R,K) > 10000.
 */
RiskResult riskAnalysisKN(const ProblemInstance& instance, int mode, int K) {
    RiskResult result;
    result.K = K;
    result.anyRisky = false;

    int R = (int)instance.reviewers.size();

    if (K >= R) {
        // Removing all reviewers always fails (trivially)
        std::vector<int> allIds;
        for (const auto& r : instance.reviewers) allIds.push_back(r.id);
        result.riskySets.push_back(allIds);
        result.anyRisky = true;
        return result;
    }

    // Warn for large search spaces
    auto combos = combinations(R, K);
    if (combos.size() > 10000)
        std::cerr << "[Warning] C(" << R << "," << K << ") = " << combos.size()
                  << " subsets to evaluate — this may take a while.\n";

    for (const auto& indices : combos) {
        // Build instance without the reviewers at these indices
        ProblemInstance reduced = instance;
        // Erase in reverse order to preserve indices
        std::vector<int> sortedIdx = indices;
        std::sort(sortedIdx.rbegin(), sortedIdx.rend());
        for (int idx : sortedIdx)
            reduced.reviewers.erase(reduced.reviewers.begin() + idx);

        if (!assignmentFeasible(reduced, mode)) {
            // Record the actual reviewer IDs (sorted)
            std::vector<int> riskyIds;
            for (int idx : indices)
                riskyIds.push_back(instance.reviewers[idx].id);
            std::sort(riskyIds.begin(), riskyIds.end());
            result.riskySets.push_back(riskyIds);
            result.anyRisky = true;
        }
    }

    // Sort the list of risky sets lexicographically
    std::sort(result.riskySets.begin(), result.riskySets.end());

    return result;
}

// ─── Output ───────────────────────────────────────────────────────────────────

/**
 * Writes the risk analysis output in the format of Figure 4:
 *
 *   #Risk Analysis: M
 *   id1, id2, ...    (one line per risky set)
 *
 * If no risky sets found, writes a comment indicating that.
 */
void writeRiskOutput(const RiskResult& result, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Cannot open risk output file: " + filename);

    out << "#Risk Analysis: " << result.K << "\n";

    if (!result.anyRisky) {
        out << "# No risky sets found — assignment is robust.\n";
    } else {
        for (const auto& set : result.riskySets) {
            for (size_t i = 0; i < set.size(); ++i) {
                if (i) out << ", ";
                out << set[i];
            }
            out << "\n";
        }
    }

    out.close();
    std::cout << "[Risk output written to: " << filename << "]\n";
}
