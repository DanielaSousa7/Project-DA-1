#ifndef RISK_ANALYSIS_H
#define RISK_ANALYSIS_H

#include "DataStructures.h"
#include <vector>
#include <string>

/**
 * @brief Result of a risk analysis run.
 */
struct RiskResult {
    int K;                                        ///< Risk parameter used
    bool anyRisky;                                ///< True if at least one risky set found
    std::vector<std::vector<int>> riskySets;      ///< Each entry = set of reviewer IDs whose
                                                  ///  simultaneous absence breaks assignment
};

/**
 * @brief Checks if a valid assignment exists for a given instance and mode.
 *        Used internally by both K=1 and K>1 analysis.
 *
 * @param instance  Problem instance (may have some reviewers removed).
 * @param mode      GenerateAssignments mode (1-3).
 * @return true if max-flow satisfies all minimum review requirements.
 *
 * Time complexity: O(V * E²) — one Edmonds-Karp run.
 */
bool assignmentFeasible(const ProblemInstance& instance, int mode);

/**
 * @brief Risk Analysis K=1 (T2.2).
 *
 * Determines which individual reviewers, if absent, make a valid assignment
 * impossible. Runs one Max-Flow per reviewer: O(R * V * E²) total.
 *
 * @param instance  Full problem instance.
 * @param mode      Assignment mode.
 * @return RiskResult with K=1 and all critical single reviewers.
 *
 * Time complexity: O(R * V * E²) where R = number of reviewers.
 */
RiskResult riskAnalysisK1(const ProblemInstance& instance, int mode);

/**
 * @brief Risk Analysis K>1 (T2.3).
 *
 * Determines which subsets of exactly K reviewers, if simultaneously absent,
 * make a valid assignment impossible. Enumerates all C(R,K) subsets and runs
 * one Max-Flow per subset.
 *
 * Complexity: O(C(R,K) * V * E²).
 * For large R and K this is exponential — practical for small K (≤3) and
 * moderate R. For K ≥ R/2 one should instead enumerate the complement.
 *
 * @param instance  Full problem instance.
 * @param mode      Assignment mode.
 * @param K         Number of reviewers to remove simultaneously.
 * @return RiskResult with all risky K-subsets of reviewer IDs.
 */
RiskResult riskAnalysisKN(const ProblemInstance& instance, int mode, int K);

/**
 * @brief Writes the risk analysis output file in the format of Figure 4.
 *
 * Format:
 *   #Risk Analysis: M
 *   id1, id2, ...    (one line per risky set, IDs sorted)
 *
 * @param result    The RiskResult to write.
 * @param filename  Output file path.
 *
 * Time complexity: O(S log S) where S = total reviewer IDs across all risky sets.
 */
void writeRiskOutput(const RiskResult& result, const std::string& filename);

#endif // RISK_ANALYSIS_H
