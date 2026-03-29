/**
 * @file Connection.cpp
 * @brief Implementation of the flow network and risk analysis.
 */

#include "Connection.h"
#include "data_structures/EdmondsKarp.h"
#include <map>
using namespace std;

/**
 * @brief Determines the domain that allows a submission-reviewer pair to match.
 *
 * Behaviour depends on the assignment mode:
 * - Mode 1: primary submission domain must equal primary reviewer expertise
 * - Mode 2: (primary or secondary) submission domain must equal primary reviewer expertise
 *
 * @param s    The submission.
 * @param r    The reviewer.
 * @param data The full problem instance (used for mode and parameter flags).
 * @return The matched domain number, or -1 if there is no valid match.
 *
 * Complexity: O(1) - constant time, performs only a few comparison operations.
 */

int getMatchDomain(Submission s, Reviewer r, AssignmentData data) {
    int mode = data.control.generateAssignments;
    bool useSubPrimary   = data.parameters.primarySubmissionDomain;
    bool useSubSecondary = data.parameters.secondarySubmissionDomain;
    bool useRevPrimary   = data.parameters.primaryReviewerExpertise;

    if (mode == 1) {
        if (useSubPrimary && useRevPrimary && s.primaryDomain == r.primaryExpertise)
            return s.primaryDomain;
    }
    else if (mode == 2) {
        if (useRevPrimary) {
            if (useSubPrimary && s.primaryDomain == r.primaryExpertise)
                return s.primaryDomain;
            if (useSubSecondary && s.secondaryDomain != -1 && s.secondaryDomain == r.primaryExpertise)
                return s.secondaryDomain;
        }
    }
    return -1;
}

/**
 * @brief Runs the review assignment algorithm using Max-Flow (Edmonds-Karp).
 *
 * Node IDs in the graph:
 * - 0: source
 * - 1: sink
 * - 1000 + submissionId: one node per submission
 * - 2000 + reviewerId: one node per reviewer
 *
 * @param data The full problem instance.
 * @param excludeReviewerId Reviewer to exclude from the network (-1 = include all).
 * @return A ReviewAssignment containing all assigned paper-reviewer pairs.
 *
 * Complexity: O(V * E^2) This is the complexity of Edmonds-Karp algorithm.
 * V = number of vertices (2 + |submissions| + |reviewers|)
 * E = number of edges (source->submission + reviewer->sink + submission->reviewer edges).
 */

ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId) {
    ReviewAssignment assignment;
    Graph<int> g;
    int source = 0;
    int sink = 1;
    int submissionOffset = 1000;
    int reviewerOffset   = 2000;

    g.addVertex(source);
    g.addVertex(sink);

    for (Submission s : data.submissions)
        g.addVertex(submissionOffset + s.id);

    for (Reviewer r : data.reviewers) {
        if (r.id == excludeReviewerId) continue;
        g.addVertex(reviewerOffset + r.id);
    }

    for (Submission s : data.submissions)
        g.addEdge(source, submissionOffset + s.id, data.parameters.minReviewsPerSubmission);

    for (Reviewer r : data.reviewers) {
        if (r.id == excludeReviewerId) continue;
        g.addEdge(reviewerOffset + r.id, sink, data.parameters.maxReviewsPerReviewer);
    }

    for (Submission s : data.submissions) {
        for (Reviewer r : data.reviewers) {
            if (r.id == excludeReviewerId) continue;
            int dom = getMatchDomain(s, r, data);
            if (dom != -1)
                g.addEdge(submissionOffset + s.id, reviewerOffset + r.id, 1);
        }
    }

    edmondsKarp(&g, source, sink);

    for (Vertex<int>* v : g.getVertexSet()) {
        for (Edge<int>* e : v->getAdj()) {
            int u = e->getOrig()->getInfo();
            int w = e->getDest()->getInfo();
            if (u < submissionOffset || u >= reviewerOffset || w < reviewerOffset) continue;

            if (e->getFlow() > 0) {
                int subId = u - submissionOffset;
                int revId = w - reviewerOffset;
                int dom = -1;
                for (Submission s : data.submissions) {
                    if (s.id != subId) continue;
                    for (Reviewer r : data.reviewers) {
                        if (r.id == revId) { dom = getMatchDomain(s, r, data); break; }
                    }
                    break;
                }
                assignment.submissionAssignments.push_back({subId, revId, dom});
                assignment.reviewerAssignments.push_back({revId, subId, dom});
                assignment.totalAssignments++;
            }
        }
    }
    return assignment;
}

/**
 * @brief Checks whether a full valid assignment is possible when reviewer is removed.
 * @param data The full problem instance.
 * @param excludeReviewerId Reviewer to exclude (-1 = include all).
 * @return true if totalAssignments is greater or equal to submissions × minReviewsPerSubmission.
 *
 * Complexity: O(V * E^2). This function inherits the complexity of the Edmonds-Karp algorithm, since that runReviewAssignment() is called.
 */

bool assignmentFeasible(AssignmentData data, int excludeReviewerId) {
    ReviewAssignment result = runReviewAssignment(data, excludeReviewerId);
    int required = (int)data.submissions.size() * data.parameters.minReviewsPerSubmission;
    return result.totalAssignments >= required;
}

/**
 * @brief Risk Analysis: finds reviewers whose individual absence breaks the assignment.
 * Start by checking if the base assignment is already infeasible, if so return an empty list
 * For each reviewer, removes them from the problem and checks whether a full
 * assignment is still possible. If not, they are considered critical.
 * @param data The full problem instance.
 * @return Sorted list of critical reviewer IDs.
 *
 * Complexity: O(R * V * E^2)
 * R = number of reviewers, V = number of vertices, E = number of edges.
 * For each reviewer, we run a full max-flow computation.
 * The final sort is O(R log R).
 */

vector<int> riskAnalysis(AssignmentData data) {
    vector<int> risky;
    if (!assignmentFeasible(data, -1)) return risky;
    for (Reviewer r : data.reviewers)
        if (!assignmentFeasible(data, r.id))
            risky.push_back(r.id);
    sort(risky.begin(), risky.end());
    return risky;
}