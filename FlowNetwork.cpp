#include "FlowNetwork.h"
#include <queue>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

// ─── Constructor: build the graph and run the algorithm ───────────────────────

FlowNetwork::FlowNetwork(const ProblemInstance& instance, int mode) {
    numSubs_ = (int)instance.submissions.size();
    numRevs_ = (int)instance.reviewers.size();

    // Node numbering:
    //   0                        → source
    //   1 .. numSubs_            → one node per paper
    //   numSubs_+1 .. numSubs_+numRevs_ → one node per reviewer
    //   numSubs_+numRevs_+1      → sink
    source_ = 0;
    sink_   = numSubs_ + numRevs_ + 1;

    for (int i = 0; i <= sink_; ++i)
        graph_.addVertex(i);

    int minRev = instance.params.minReviewsPerSubmission;
    int maxRev = instance.params.maxReviewsPerReviewer;

    // Source → each paper (capacity = minimum reviews needed)
    for (int i = 0; i < numSubs_; ++i)
        addFlowEdge(source_, submissionNode(i), minRev);

    // Each paper → each compatible reviewer (capacity = 1)
    for (int i = 0; i < numSubs_; ++i)
        for (int j = 0; j < numRevs_; ++j)
            if (matchedDomain(instance.submissions[i], instance.reviewers[j], mode) > 0)
                addFlowEdge(submissionNode(i), reviewerNode(j), 1);

    // Each reviewer → sink (capacity = maximum reviews they can do)
    for (int j = 0; j < numRevs_; ++j)
        addFlowEdge(reviewerNode(j), sink_, maxRev);

    // Run the algorithm
    int flow = edmondsKarp();
    result_.totalFlow = flow;
    result_.success   = (flow == numSubs_ * minRev);

    // Always extract what was assigned (even if partial)
    extractAssignments(instance, mode);

    // If incomplete, also report how many reviews each paper is still missing
    if (!result_.success) {
        auto* src = graph_.findVertex(source_);
        for (int i = 0; i < numSubs_; ++i) {
            int assigned = 0;
            for (auto* e : src->getAdj())
                if (e->getDest()->getInfo() == submissionNode(i))
                    { assigned = (int)e->getFlow(); break; }

            int missing = minRev - assigned;
            if (missing > 0)
                result_.missing.emplace_back(
                    instance.submissions[i].id,
                    instance.submissions[i].primaryTopic,
                    missing);
        }
    }
}

// ─── Accessors ────────────────────────────────────────────────────────────────

const FlowResult& FlowNetwork::getResult() const { return result_; }
const Graph<int>& FlowNetwork::getGraph()  const { return graph_;  }
int FlowNetwork::sourceNode()         const { return source_; }
int FlowNetwork::sinkNode()           const { return sink_;   }
int FlowNetwork::submissionNode(int i) const { return i + 1; }
int FlowNetwork::reviewerNode(int j)   const { return numSubs_ + j + 1; }

// ─── addFlowEdge ──────────────────────────────────────────────────────────────

// Creates a forward edge u→v (capacity = cap) and a backward edge v→u
// (capacity = 0). The backward edge is used by the algorithm to "undo" flow.
void FlowNetwork::addFlowEdge(int u, int v, double cap) {
    auto* vu = graph_.findVertex(u);
    auto* vv = graph_.findVertex(v);
    if (!vu || !vv) throw runtime_error("addFlowEdge: vertex not found");

    Edge<int>* fwd = vu->addEdge(vv, cap);  fwd->setFlow(0);
    Edge<int>* rev = vv->addEdge(vu, 0);    rev->setFlow(0);
    fwd->setReverse(rev);
    rev->setReverse(fwd);
}

// ─── matchedDomain ────────────────────────────────────────────────────────────

// Checks whether a paper and a reviewer share a compatible domain.
// Returns the matched domain number (> 0) if compatible, 0 otherwise.
//
// Mode 1: only primary paper domain ↔ primary reviewer expertise
// Mode 2: (primary or secondary) paper ↔ primary reviewer expertise
// Mode 3: all combinations — primary and secondary on both sides
int FlowNetwork::matchedDomain(const Submission& s, const Reviewer& r, int mode) {
    int sp = s.primaryTopic,      ss = s.secondaryTopic;   // paper domains
    int rp = r.primaryExpertise,  rs = r.secondaryExpertise; // reviewer expertise

    if (mode == 1) {
        return (sp == rp) ? sp : 0;
    }
    if (mode == 2) {
        if (sp == rp)       return sp;
        if (ss && ss == rp) return ss;
        return 0;
    }
    if (mode == 3) {
        if (sp == rp)       return sp;
        if (ss && ss == rp) return ss;
        if (rs && sp == rs) return sp;
        if (rs && ss && ss == rs) return ss;
        return 0;
    }
    return 0;
}

// ─── BFS: find an augmenting path ─────────────────────────────────────────────

// Searches for a path from source to sink through edges that still have
// remaining capacity (residual edges). Sets vertex.path to reconstruct the path.
// Returns true if such a path exists.
// Time complexity: O(V + E)
bool FlowNetwork::bfsAugmentingPath() {
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
            auto* v = e->getDest();
            double remaining = e->getWeight() - e->getFlow();
            if (!v->isVisited() && remaining > 0) {
                v->setVisited(true);
                v->setPath(e);
                if (v->getInfo() == sink_) return true;
                q.push(v);
            }
        }
    }
    return false;
}

// ─── Edmonds-Karp: maximum flow algorithm ────────────────────────────────────

// Repeatedly finds the shortest augmenting path (BFS) and pushes as much
// flow as possible through it, until no more paths exist.
// Time complexity: O(V × E²)
int FlowNetwork::edmondsKarp() {
    int totalFlow = 0;

    while (bfsAugmentingPath()) {
        // Find the bottleneck: the minimum remaining capacity along the path
        double bottleneck = numeric_limits<double>::max();
        auto* v = graph_.findVertex(sink_);
        while (v->getInfo() != source_) {
            auto* e = v->getPath();
            bottleneck = min(bottleneck, e->getWeight() - e->getFlow());
            v = e->getOrig();
        }

        // Push flow along the path and update reverse edges
        v = graph_.findVertex(sink_);
        while (v->getInfo() != source_) {
            auto* e = v->getPath();
            e->setFlow(e->getFlow() + bottleneck);
            if (e->getReverse())
                e->getReverse()->setFlow(e->getReverse()->getFlow() - bottleneck);
            v = e->getOrig();
        }

        totalFlow += (int)bottleneck;
    }
    return totalFlow;
}

// ─── Extract assignments from the final flow ──────────────────────────────────

// After the algorithm finishes, any paper→reviewer edge with flow = 1
// represents an actual assignment.
void FlowNetwork::extractAssignments(const ProblemInstance& instance, int mode) {
    for (int i = 0; i < numSubs_; ++i) {
        auto* subNode = graph_.findVertex(submissionNode(i));
        for (auto* e : subNode->getAdj()) {
            if ((int)e->getFlow() == 1) {
                int j = e->getDest()->getInfo() - numSubs_ - 1; // reviewer index
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

    // Sort by paper ID, then reviewer ID
    sort(result_.assignments.begin(), result_.assignments.end(),
         [](const Assignment& a, const Assignment& b) {
             return a.submissionId != b.submissionId
                 ? a.submissionId < b.submissionId
                 : a.reviewerId   < b.reviewerId;
         });
}

// ─── Write output file ────────────────────────────────────────────────────────

void writeAssignmentOutput(const FlowResult& result,
                           const string& filename,
                           const vector<string>& riskLines) {
    ofstream out(filename);
    if (!out.is_open())
        throw runtime_error("Cannot open output file: " + filename);

    // Always show what was assigned (even if partial)
    if (!result.assignments.empty()) {
        out << "#SubmissionId,ReviewerId,Match\n";
        for (const auto& a : result.assignments)
            out << a.submissionId << ", " << a.reviewerId << ", " << a.matchDomain << "\n";

        map<int, vector<pair<int,int>>> byReviewer;
        for (const auto& a : result.assignments)
            byReviewer[a.reviewerId].emplace_back(a.submissionId, a.matchDomain);

        out << "#ReviewerId,SubmissionId,Match\n";
        for (auto& [revId, papers] : byReviewer) {
            sort(papers.begin(), papers.end());
            for (auto& [subId, dom] : papers)
                out << revId << ", " << subId << ", " << dom << "\n";
        }

        out << "#Total: " << result.totalFlow << "\n";
    }

    // If incomplete, also show missing reviews
    if (!result.success) {
        out << "#SubmissionId,Domain,MissingReviews\n";
        auto missing = result.missing;
        sort(missing.begin(), missing.end());
        for (auto& [subId, dom, cnt] : missing)
            out << subId << ", " << dom << ", " << cnt << "\n";
    }

    // Append risk analysis lines to the same file if provided
    for (const auto& line : riskLines)
        out << line << "\n";

    out.close();
    cout << "[Output written to: " << filename << "]\n";
}