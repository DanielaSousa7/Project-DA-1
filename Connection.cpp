#include "Connection.h"
#include "data_structures/EdmondsKarp.h"
#include <map>

using namespace std;

ReviewAssignment runReviewAssignment(AssignmentData data) {
    ReviewAssignment assignment;
    Graph<int> g;
    int source = 0;
    int sink = 1;
    int submissionOffset = 1000;
    int reviewerOffset = 2000;
    g.addVertex(source);
    g.addVertex(sink);
    map<int, pair<pair<int, int>, int>> submissionDomain;
    for (Submission s : data.submissions) {
        g.addVertex(submissionOffset + s.id);
        submissionDomain[s.id] = {{s.primaryDomain, s.secondaryDomain}, -1};
    }
    for (Reviewer r : data.reviewers) {
        g.addVertex(reviewerOffset + r.id);
    }
    for (Submission s : data.submissions) {
        g.addEdge(source, submissionOffset + s.id, data.parameters.minReviewsPerSubmission);
    }
    for (auto &r : data.reviewers) {
        g.addEdge(reviewerOffset + r.id, sink, data.parameters.maxReviewsPerReviewer);
    }
    for (auto &s : data.submissions) {
        for (auto &r : data.reviewers) {
            bool match = false;

            bool useSubPrimary   = data.parameters.primarySubmissionDomain;
            bool useSubSecondary = data.parameters.secondarySubmissionDomain;
            bool useRevPrimary   = data.parameters.primaryReviewerExpertise;

            int mode = data.control.generateAssignments;
            if (mode == 1) {
                if (useSubPrimary && useRevPrimary) {
                    match = (s.primaryDomain == r.primaryExpertise);
                    submissionDomain[s.id].second = 0;
                }
            }
            else if (mode == 2) {
                if (useRevPrimary) {
                    if (useSubPrimary && s.primaryDomain == r.primaryExpertise) {
                        match = true;
                        submissionDomain[s.id].second = 0;
                    }
                    if (useSubSecondary && s.secondaryDomain != -1 &&s.secondaryDomain == r.primaryExpertise) {
                        match = true;
                        submissionDomain[s.id].second = 1;
                    }
                }
            }
            if (match) {
                g.addEdge(submissionOffset + s.id, reviewerOffset + r.id, 1);
            }
        }
    }
    edmondsKarp(&g, source, sink);
    int totalFlow = 0;
    for (Edge<int>* e : g.findVertex(source)->getAdj()) {
        totalFlow += e->getFlow();
    }
    for (Vertex<int>* v : g.getVertexSet()) {
        for (Edge<int>* e : v->getAdj()) {
            int u = e->getOrig()->getInfo();
            int w = e->getDest()->getInfo();
            if (u >= submissionOffset && u < reviewerOffset && w >= reviewerOffset && e->getFlow() > 0) {
                int subId = u - submissionOffset;
                int revId = w - reviewerOffset;

                int matchDomainLevel = submissionDomain[subId].second;
                int matchDomain;
                switch (matchDomainLevel) {
                    case 0 : matchDomain = submissionDomain[subId].first.first; break;
                    case 1 : matchDomain = submissionDomain[subId].first.second; break;
                }
                assignment.submissionAssignments.push_back({subId, revId, matchDomain});
                assignment.reviewerAssignments.push_back({revId, subId, matchDomain});
                assignment.totalAssignments++;
            }
        }
    }
    return assignment;
}