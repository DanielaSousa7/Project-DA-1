#include "Connection.h"
#include "data_structures/EdmondsKarp.h"
#include <map>
using namespace std;

// Returns which domain matched between submission s and reviewer r under the given mode.
// Returns -1 if no match.
static int getMatchDomain(const Submission& s, const Reviewer& r, const AssignmentData& data) {
    int mode = data.control.generateAssignments;
    bool useSubPrimary   = data.parameters.primarySubmissionDomain;
    bool useSubSecondary = data.parameters.secondarySubmissionDomain;
    bool useRevPrimary   = data.parameters.primaryReviewerExpertise;
    bool useRevSecondary = data.parameters.secondaryReviewerExpertise;

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
    else if (mode == 3) {
        if (useSubPrimary   && useRevPrimary   && s.primaryDomain   == r.primaryExpertise)   return s.primaryDomain;
        if (useSubSecondary && useRevPrimary   && s.secondaryDomain != -1 && s.secondaryDomain == r.primaryExpertise)  return s.secondaryDomain;
        if (useSubPrimary   && useRevSecondary && r.secondaryExpertise != -1 && s.primaryDomain   == r.secondaryExpertise) return s.primaryDomain;
        if (useSubSecondary && useRevSecondary && s.secondaryDomain != -1 && r.secondaryExpertise != -1 && s.secondaryDomain == r.secondaryExpertise) return s.secondaryDomain;
    }
    return -1;
}

// Builds and solves the flow network. excludeReviewerId = -1 means include all reviewers.
ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId) {
    ReviewAssignment assignment;
    Graph<int> g;

    const int source          = 0;
    const int sink            = 1;
    const int submissionOffset = 1000;
    const int reviewerOffset   = 2000;

    g.addVertex(source);
    g.addVertex(sink);

    for (const Submission& s : data.submissions)
        g.addVertex(submissionOffset + s.id);

    for (const Reviewer& r : data.reviewers) {
        if (r.id == excludeReviewerId) continue;
        g.addVertex(reviewerOffset + r.id);
    }

    // Source → each submission (capacity = min reviews needed)
    for (const Submission& s : data.submissions)
        g.addEdge(source, submissionOffset + s.id, data.parameters.minReviewsPerSubmission);

    // Each reviewer → sink (capacity = max reviews allowed)
    for (const Reviewer& r : data.reviewers) {
        if (r.id == excludeReviewerId) continue;
        g.addEdge(reviewerOffset + r.id, sink, data.parameters.maxReviewsPerReviewer);
    }

    // Submission → reviewer edges (only if domains match)
    for (const Submission& s : data.submissions) {
        for (const Reviewer& r : data.reviewers) {
            if (r.id == excludeReviewerId) continue;
            int dom = getMatchDomain(s, r, data);
            if (dom != -1)
                g.addEdge(submissionOffset + s.id, reviewerOffset + r.id, 1);
        }
    }

    edmondsKarp(&g, source, sink);

    // Extract assignments from edges with flow > 0
    for (Vertex<int>* v : g.getVertexSet()) {
        for (Edge<int>* e : v->getAdj()) {
            int u = e->getOrig()->getInfo();
            int w = e->getDest()->getInfo();
            if (u >= submissionOffset && u < reviewerOffset &&
                w >= reviewerOffset   && e->getFlow() > 0) {

                int subId = u - submissionOffset;
                int revId = w - reviewerOffset;

                // Find the actual matched domain
                int dom = -1;
                for (const Submission& s : data.submissions) {
                    if (s.id != subId) continue;
                    for (const Reviewer& r : data.reviewers) {
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

// Checks if a valid full assignment is possible (all papers get minReviews)
bool assignmentFeasible(AssignmentData data, int excludeReviewerId) {
    ReviewAssignment result = runReviewAssignment(data, excludeReviewerId);
    int required = (int)data.submissions.size() * data.parameters.minReviewsPerSubmission;
    return result.totalAssignments >= required;
}

// Risk Analysis K=1: finds which single reviewer's absence breaks the assignment
vector<int> riskAnalysisK1(AssignmentData data) {
    vector<int> risky;
    // Only meaningful if the base assignment is feasible
    if (!assignmentFeasible(data, -1)) return risky;

    for (const Reviewer& r : data.reviewers)
        if (!assignmentFeasible(data, r.id))
            risky.push_back(r.id);

    sort(risky.begin(), risky.end());
    return risky;
}
