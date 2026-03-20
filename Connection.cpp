#include "Connection.h"
#include "data_structures/EdmondsKarp.h"
#include <map>

using namespace std;

ReviewAssignment runReviewAssignment(const AssignmentData &data) {
    ReviewAssignment assignment;

    Graph<int> g;

    // IDs especiais
    int source = 0;
    int sink = 1;

    int submissionOffset = 1000;
    int reviewerOffset = 2000;

    g.addVertex(source);
    g.addVertex(sink);

    // Map para guardar domain das submissions
    map<int,int> submissionDomain;

    // 1️⃣ Submissions
    for (auto &s : data.submissions) {
        g.addVertex(submissionOffset + s.id);
        submissionDomain[s.id] = s.primaryDomain;
    }

    // 2️⃣ Reviewers
    for (auto &r : data.reviewers) {
        g.addVertex(reviewerOffset + r.id);
    }

    // 3️⃣ source -> submissions
    for (auto &s : data.submissions) {
        g.addEdge(source, submissionOffset + s.id,
                  data.parameters.minReviewsPerSubmission);
    }

    // 4️⃣ reviewers -> sink
    for (auto &r : data.reviewers) {
        g.addEdge(reviewerOffset + r.id, sink,
                  data.parameters.maxReviewsPerReviewer);
    }

    // 5️⃣ submission -> reviewer (matching)
for (auto &s : data.submissions) {
    for (auto &r : data.reviewers) {

        bool match = false;

        // flags dos parameters
        bool useSubPrimary   = data.parameters.primarySubmissionDomain;
        bool useSubSecondary = data.parameters.secondarySubmissionDomain;
        bool useRevPrimary   = data.parameters.primaryReviewerExpertise;
        bool useRevSecondary = data.parameters.secondaryReviewerExpertise;

        int mode = data.control.generateAssignments;

        // 🔴 CASO 1: só primary-primary
        if (mode == 1) {
            if (useSubPrimary && useRevPrimary) {
                match = (s.primaryDomain == r.primaryExpertise);
            }
        }

        // 🔴 CASO 2: submission pode ter secondary
        else if (mode == 2) {

            if (useRevPrimary) {
                if (useSubPrimary && s.primaryDomain == r.primaryExpertise)
                    match = true;

                if (useSubSecondary && s.secondaryDomain != -1 &&
                    s.secondaryDomain == r.primaryExpertise)
                    match = true;
            }
        }

        // 🔴 CASO 3: tudo permitido
        else if (mode == 3) {

            // primary-primary
            if (useSubPrimary && useRevPrimary &&
                s.primaryDomain == r.primaryExpertise)
                match = true;

            // secondary submission → primary reviewer
            if (useSubSecondary && useRevPrimary &&
                s.secondaryDomain != -1 &&
                s.secondaryDomain == r.primaryExpertise)
                match = true;

            // primary submission → secondary reviewer
            if (useSubPrimary && useRevSecondary &&
                r.secondaryExpertise != -1 &&
                s.primaryDomain == r.secondaryExpertise)
                match = true;

            // secondary-secondary
            if (useSubSecondary && useRevSecondary &&
                s.secondaryDomain != -1 &&
                r.secondaryExpertise != -1 &&
                s.secondaryDomain == r.secondaryExpertise)
                match = true;
        }

        if (match) {
            g.addEdge(submissionOffset + s.id,
                      reviewerOffset + r.id,
                      1);
        }
    }
}

    // 6️⃣ Max Flow
    edmondsKarp(&g, source, sink);

    // 7️⃣ Verificar fluxo total
    int totalFlow = 0;
    for (auto e : g.findVertex(source)->getAdj()) {
        totalFlow += e->getFlow();
    }

    int totalRequired =
        data.submissions.size() * data.parameters.minReviewsPerSubmission;

    // 8️⃣ Extrair solução (se existir)
    for (auto v : g.getVertexSet()) {
        for (auto e : v->getAdj()) {

            int u = e->getOrig()->getInfo();
            int w = e->getDest()->getInfo();

            // só edges submission -> reviewer
            if (u >= submissionOffset && u < reviewerOffset &&
                w >= reviewerOffset &&
                e->getFlow() > 0) {

                int subId = u - submissionOffset;
                int revId = w - reviewerOffset;

                int matchDomain = submissionDomain[subId];

                assignment.submissionAssignments.push_back(
                    {subId, revId, matchDomain}
                );

                assignment.reviewerAssignments.push_back(
                    {revId, subId, matchDomain}
                );

                assignment.totalAssignments++;
            }
        }
    }

    return assignment;
}