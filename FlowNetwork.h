#ifndef FLOW_NETWORK_H
#define FLOW_NETWORK_H

#include "data_structures/Graph.h"
#include "DataStructures.h"
#include <vector>
#include <tuple>
#include <string>

using namespace std;

struct Assignment {
    int submissionId;
    int reviewerId;
    int matchDomain;
};

struct FlowResult {
    bool success;
    int  totalFlow;
    vector<Assignment> assignments;
    vector<tuple<int,int,int>> missing; // {paperId, domain, missingCount}
};

// Builds and solves the flow network using Edmonds-Karp.
// Node layout:
//   0 = source
//   1..numSubs = submissions
//   numSubs+1..numSubs+numRevs = reviewers
//   numSubs+numRevs+1 = sink
//
// Time complexity: O(V x E²)
class FlowNetwork {
public:
    FlowNetwork(const ProblemInstance& instance, int mode);

    const FlowResult& getResult() const;
    const Graph<int>& getGraph()  const;

    int sourceNode()          const;
    int sinkNode()            const;
    int submissionNode(int i) const;
    int reviewerNode(int j)   const;

private:
    Graph<int>  graph_;
    FlowResult  result_;
    int source_, sink_, numSubs_, numRevs_;

    void addFlowEdge(int u, int v, double capacity);
    static int matchedDomain(const Submission& s, const Reviewer& r, int mode);
    bool bfsAugmentingPath();
    int  edmondsKarp();
    void extractAssignments(const ProblemInstance& instance, int mode);
};

// Write assignment output to file.
// If riskLines is provided, appends them at the end of the same file.
void writeAssignmentOutput(const FlowResult& result,
                           const string& filename,
                           const vector<string>& riskLines = {});

#endif