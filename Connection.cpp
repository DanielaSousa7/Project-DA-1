#include "Connection.h"
#include "data_structures/EdmondsKarp.h"
#include <map>
using namespace std;

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
            if (u >= submissionOffset && u < reviewerOffset &&
                w >= reviewerOffset   && e->getFlow() > 0) {
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

bool assignmentFeasible(AssignmentData data, int excludeReviewerId) {
    ReviewAssignment result = runReviewAssignment(data, excludeReviewerId);
    int required = (int)data.submissions.size() * data.parameters.minReviewsPerSubmission;
    return result.totalAssignments >= required;
}

vector<int> riskAnalysis(AssignmentData data) {
    vector<int> risky;
    if (!assignmentFeasible(data, -1)) return risky;
    for (Reviewer r : data.reviewers)
        if (!assignmentFeasible(data, r.id))
            risky.push_back(r.id);
    sort(risky.begin(), risky.end());
    return risky;
}