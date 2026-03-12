#ifndef FLOW_NETWORK_H
#define FLOW_NETWORK_H

#include "data_structures/Graph.h"
#include "DataStructures.h"
#include <vector>
#include <tuple>
#include <string>

/**
 * @brief Represents one review assignment: paper → reviewer with matched domain.
 */
struct Assignment {
    int submissionId;  ///< Paper ID
    int reviewerId;    ///< Reviewer ID
    int matchDomain;   ///< The domain that matched (submission side)
};

/**
 * @brief Result of the flow computation.
 */
struct FlowResult {
    bool success;                        ///< True if all submissions got MinReviews
    int  totalFlow;                      ///< Total number of review assignments made
    std::vector<Assignment> assignments; ///< The actual assignments (success case)
    /// For the failure case: {submissionId, domain, missingCount}
    std::vector<std::tuple<int,int,int>> missing;
};

/**
 * @brief Builds a Max-Flow network from a ProblemInstance and solves it
 *        using the Edmonds-Karp algorithm (BFS-based Ford-Fulkerson).
 *
 * Node layout inside Graph<int>:
 *   0               → source (S)
 *   1 .. |subs|     → one node per submission
 *   |subs|+1 .. |subs|+|revs| → one node per reviewer
 *   |subs|+|revs|+1 → sink (T)
 *
 * Edges:
 *   S  → submission_i  : capacity = MinReviewsPerSubmission
 *   sub_i → reviewer_j : capacity = 1  (only if domains match per mode)
 *   reviewer_j → T      : capacity = MaxReviewsPerReviewer
 *
 * The assignment mode (GenerateAssignments 1-3) controls which domain
 * combinations are considered when creating sub→rev edges.
 *
 * Time complexity: O(V * E²) for Edmonds-Karp, where V and E are the
 * number of vertices and edges in the flow network.
 */
class FlowNetwork {
public:
    /**
     * @brief Constructs and solves the flow network.
     * @param instance  The parsed problem instance.
     * @param mode      GenerateAssignments value (1, 2, or 3).
     *                  1 = primary-only; 2 = sub primary+secondary, rev primary;
     *                  3 = all combinations.
     *
     * After construction, call getResult() to retrieve assignments.
     */
    FlowNetwork(const ProblemInstance& instance, int mode);

    /**
     * @brief Returns the result of the assignment computation.
     */
    const FlowResult& getResult() const;

    /**
     * @brief Returns the underlying flow graph (for inspection / risk analysis).
     */
    const Graph<int>& getGraph() const;

    /**
     * @brief Returns the node index in the graph for a given submission (by position).
     * @param subIndex  0-based index into instance.submissions.
     */
    int submissionNode(int subIndex) const;

    /**
     * @brief Returns the node index in the graph for a given reviewer (by position).
     * @param revIndex  0-based index into instance.reviewers.
     */
    int reviewerNode(int revIndex) const;

    int sourceNode() const;
    int sinkNode()   const;

private:
    Graph<int> graph_;
    FlowResult result_;

    int source_;
    int sink_;
    int numSubs_;
    int numRevs_;

    /**
     * @brief Adds a directed flow edge with a reverse edge of capacity 0.
     *        Links them via setReverse so Edmonds-Karp can update residuals.
     * @param u  Source node id.
     * @param v  Destination node id.
     * @param cap  Forward capacity.
     *
     * Time complexity: O(1) amortised.
     */
    void addFlowEdge(int u, int v, double cap);

    /**
     * @brief Checks whether submission s and reviewer r share a domain
     *        given the assignment mode.
     * @return The matched domain (> 0) if compatible, 0 otherwise.
     *
     * Time complexity: O(1).
     */
    static int matchedDomain(const Submission& s, const Reviewer& r, int mode);

    /**
     * @brief BFS on the residual graph to find an augmenting path from
     *        source to sink. Sets vertex.path and vertex.visited.
     * @return True if a path exists.
     *
     * Time complexity: O(V + E).
     */
    bool bfsAugmentingPath();

    /**
     * @brief Runs Edmonds-Karp (iterative BFS Ford-Fulkerson).
     * @return Total max-flow value.
     *
     * Time complexity: O(V * E²).
     */
    int edmondsKarp();

    /**
     * @brief Extracts assignment pairs from the final flow values on
     *        submission→reviewer edges.
     */
    void extractAssignments(const ProblemInstance& instance, int mode);
};

/**
 * @brief Writes the assignment output file in the format specified by the project.
 * @param result    The FlowResult to write.
 * @param instance  The problem instance (for sorting submission/reviewer IDs).
 * @param filename  Output file path.
 *
 * Time complexity: O(A log A) where A = number of assignments (due to sorting).
 */
void writeAssignmentOutput(const FlowResult& result,
                           const ProblemInstance& instance,
                           const std::string& filename);

#endif // FLOW_NETWORK_H
