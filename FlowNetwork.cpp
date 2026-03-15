#include "FlowNetwork.h"

#include <queue>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

// ─── Construction ─────────────────────────────────────────────────────────────

FlowNetwork::FlowNetwork(const ProblemInstance& instance, int mode) {
    numSubs_ = (int)instance.submissions.size();
    numRevs_ = (int)instance.reviewers.size();

    // Node IDs:
    //   0               → source
    //   1 .. numSubs_   → submissions
    //   numSubs_+1 .. numSubs_+numRevs_ → reviewers
    //   numSubs_+numRevs_+1 → sink
    source_ = 0;
    sink_   = numSubs_ + numRevs_ + 1;

    // Add all vertices to the graph
    for (int i = 0; i <= sink_; ++i)
        graph_.addVertex(i);

    const int minRev = instance.params.minReviewsPerSubmission;
    const int maxRev = instance.params.maxReviewsPerReviewer;

    // Source → each submission (capacity = minReviewsPerSubmission)
    for (int i = 0; i < numSubs_; ++i)
        addFlowEdge(source_, submissionNode(i), minRev);

    // Submission → reviewer (capacity = 1, only if domains match)
    for (int i = 0; i < numSubs_; ++i) {
        for (int j = 0; j < numRevs_; ++j) {
            int dom = matchedDomain(instance.submissions[i], instance.reviewers[j], mode);
            if (dom > 0)
                addFlowEdge(submissionNode(i), reviewerNode(j), 1);
        }
    }

    // Each reviewer → sink (capacity = maxReviewsPerReviewer)
    for (int j = 0; j < numRevs_; ++j)
        addFlowEdge(reviewerNode(j), sink_, maxRev);

    // Run Edmonds-Karp
    int flow = edmondsKarp();
    result_.totalFlow = flow;
    result_.success   = (flow == numSubs_ * minRev);

    if (result_.success) {
        extractAssignments(instance, mode);
    } else {
        // Report how many reviews each submission is still missing
        for (int i = 0; i < numSubs_; ++i) {
            // Flow on source→sub edge = reviews assigned so far
            int assigned = 0;
            // Find the source→sub edge via source's adjacency list
            auto* srcVertex = graph_.findVertex(source_);
            for (auto* e : srcVertex->getAdj()) {
                if (e->getDest()->getInfo() == submissionNode(i)) {
                    assigned = (int)e->getFlow();
                    break;
                }
            }
            int missing = minRev - assigned;
            if (missing > 0) {
                int domain = instance.submissions[i].primaryTopic;
                result_.missing.emplace_back(instance.submissions[i].id, domain, missing);
            }
        }
    }
}

// ─── Accessors ────────────────────────────────────────────────────────────────

const FlowResult& FlowNetwork::getResult() const { return result_; }
const Graph<int>& FlowNetwork::getGraph()  const { return graph_; }
int FlowNetwork::submissionNode(int i) const { return i + 1; }
int FlowNetwork::reviewerNode(int j)  const { return numSubs_ + j + 1; }
int FlowNetwork::sourceNode()         const { return source_; }
int FlowNetwork::sinkNode()           const { return sink_; }

// ─── addFlowEdge ─────────────────────────────────────────────────────────────

/**
 * Adds a directed capacity edge u→v with a paired reverse edge v→u (cap=0).
 * The two edges point to each other via setReverse() so the residual graph
 * can be updated in O(1) during augmentation.
 */
void FlowNetwork::addFlowEdge(int u, int v, double cap) {
    auto* vu = graph_.findVertex(u);
    auto* vv = graph_.findVertex(v);
    if (!vu || !vv)
        throw runtime_error("addFlowEdge: vertex not found");

    // Forward edge: capacity = cap, flow = 0
    Edge<int>* fwd = vu->addEdge(vv, cap);
    fwd->setFlow(0);

    // Reverse edge: capacity = 0 (residual), flow = 0
    Edge<int>* rev = vv->addEdge(vu, 0);
    rev->setFlow(0);

    fwd->setReverse(rev);
    rev->setReverse(fwd);
}

// ─── matchedDomain ────────────────────────────────────────────────────────────

/**
 * Returns the domain that allowed the match (> 0), or 0 if incompatible.
 *
 * Mode 1: primary submission domain ↔ primary reviewer expertise only.
 * Mode 2: (primary or secondary) submission domain ↔ primary reviewer expertise.
 * Mode 3: (primary or secondary) submission ↔ (primary or secondary) reviewer.
 */
int FlowNetwork::matchedDomain(const Submission& s, const Reviewer& r, int mode) {
    int sp = s.primaryTopic;
    int ss = s.secondaryTopic;   // 0 if undefined
    int rp = r.primaryExpertise;
    int rs = r.secondaryExpertise; // 0 if undefined

    switch (mode) {
        case 1:
            if (sp == rp) return sp;
            return 0;

        case 2:
            if (sp == rp) return sp;
            if (ss && ss == rp) return ss;
            return 0;

        case 3:
            if (sp == rp) return sp;
            if (ss && ss == rp) return ss;
            if (rs && sp == rs) return sp;
            if (rs && ss && ss == rs) return ss;
            return 0;

        default:
            return 0;
    }
}

// ─── BFS augmenting path ─────────────────────────────────────────────────────

/**
 * Standard BFS on the residual graph.
 * An edge e (u→v) is residual if:  e.weight - e.flow > 0   (forward)
 *                             or   e.flow > 0               (backward, via e.reverse)
 *
 * Uses vertex.visited and vertex.path (already in Graph.h).
 */
bool FlowNetwork::bfsAugmentingPath() {
    // Reset visited / path for all vertices
    for (auto* v : graph_.getVertexSet()) {
        v->setVisited(false);
        v->setPath(nullptr);
    }

    auto* src = graph_.findVertex(source_);
    src->setVisited(true);

    queue<Vertex<int>*> q;
    q.push(src);

    while (!q.empty()) {
        auto* u = q.front(); q.pop();

        for (auto* e : u->getAdj()) {
            double residual = e->getWeight() - e->getFlow();
            auto* v = e->getDest();
            if (!v->isVisited() && residual > 0) {
                v->setVisited(true);
                v->setPath(e);          // remember which edge we came through
                if (v->getInfo() == sink_) return true;
                q.push(v);
            }
        }
    }
    return false;
}

// ─── Edmonds-Karp ─────────────────────────────────────────────────────────────

/**
 * Iterates BFS augmenting paths until none remain.
 * Each iteration:
 *   1. BFS to find shortest augmenting path (s → t).
 *   2. Find bottleneck (minimum residual capacity along path).
 *   3. Augment: update flow on each edge (and its reverse) along the path.
 *
 * Total time complexity: O(V * E²).
 */
int FlowNetwork::edmondsKarp() {
    int totalFlow = 0;

    while (bfsAugmentingPath()) {
        // ── Find bottleneck ──────────────────────────────────────────────────
        double bottleneck = numeric_limits<double>::max();
        auto* v = graph_.findVertex(sink_);
        while (v->getInfo() != source_) {
            Edge<int>* e = v->getPath();
            double res = e->getWeight() - e->getFlow();
            bottleneck = min(bottleneck, res);
            v = e->getOrig();
        }

        // ── Augment along path ───────────────────────────────────────────────
        v = graph_.findVertex(sink_);
        while (v->getInfo() != source_) {
            Edge<int>* e = v->getPath();
            e->setFlow(e->getFlow() + bottleneck);
            if (e->getReverse())
                e->getReverse()->setFlow(e->getReverse()->getFlow() - bottleneck);
            v = e->getOrig();
        }

        totalFlow += (int)bottleneck;
    }
    return totalFlow;
}

// ─── Extract assignments ──────────────────────────────────────────────────────

/**
 * After Edmonds-Karp, any submission→reviewer edge with flow = 1 is an assignment.
 * We re-compute the matched domain for the output.
 */
void FlowNetwork::extractAssignments(const ProblemInstance& instance, int mode) {
    for (int i = 0; i < numSubs_; ++i) {
        auto* subVertex = graph_.findVertex(submissionNode(i));
        for (auto* e : subVertex->getAdj()) {
            if ((int)e->getFlow() == 1) {
                int revNode = e->getDest()->getInfo();
                int j = revNode - numSubs_ - 1; // reviewer index
                if (j < 0 || j >= numRevs_) continue;

                int dom = matchedDomain(instance.submissions[i], instance.reviewers[j], mode);
                result_.assignments.push_back({
                    instance.submissions[i].id,
                    instance.reviewers[j].id,
                    dom
                });
            }
        }
    }

    // Sort by submission ID, then reviewer ID
    sort(result_.assignments.begin(), result_.assignments.end(),
              [](const Assignment& a, const Assignment& b) {
                  if (a.submissionId != b.submissionId) return a.submissionId < b.submissionId;
                  return a.reviewerId < b.reviewerId;
              });
}

// ─── Write output file ────────────────────────────────────────────────────────

/**
 * Writes the output in the format specified in the project (Figures 2 and 3).
 */
void writeAssignmentOutput(const FlowResult& result,
                           const string& filename) {
    ofstream out(filename);
    if (!out.is_open())
        throw runtime_error("Cannot open output file: " + filename);

    if (result.success) {
        // ── Section 1: submission → reviewer ────────────────────────────────
        out << "#SubmissionId,ReviewerId,Match\n";
        for (const auto& a : result.assignments)
            out << a.submissionId << ", " << a.reviewerId << ", " << a.matchDomain << "\n";

        // ── Section 2: reviewer → submissions (dual view) ───────────────────
        // Build map: reviewerId → list of {submissionId, domain}
        map<int, vector<pair<int,int>>> revMap;
        for (const auto& a : result.assignments)
            revMap[a.reviewerId].emplace_back(a.submissionId, a.matchDomain);

        out << "#ReviewerId,SubmissionId,Match\n";
        for (auto& [revId, subs] : revMap) {
            sort(subs.begin(), subs.end());
            for (auto& [subId, dom] : subs)
                out << revId << ", " << subId << ", " << dom << "\n";
        }

        out << "#Total: " << result.totalFlow << "\n";

    } else {
        // ── Unsuccessful: report missing reviews ─────────────────────────────
        out << "#SubmissionId,Domain,MissingReviews\n";
        auto missing = result.missing;
        sort(missing.begin(), missing.end());
        for (auto& [subId, dom, cnt] : missing)
            out << subId << ", " << dom << ", " << cnt << "\n";
    }

    out.close();
    cout << "[Output written to: " << filename << "]\n";
}