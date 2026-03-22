#include "Output.h"
#include "Connection.h"
#include <fstream>
#include <algorithm>
using namespace std;

void printAssignment(ReviewAssignment assignment, AssignmentData data, string filename) {
    ofstream out(filename);

    auto sub = assignment.submissionAssignments;
    auto rev = assignment.reviewerAssignments;

    sort(sub.begin(), sub.end(), [](const auto& a, const auto& b) {
        return a.submissionId != b.submissionId
            ? a.submissionId < b.submissionId
            : a.reviewerId   < b.reviewerId;
    });
    sort(rev.begin(), rev.end(), [](const auto& a, const auto& b) {
        return a.reviewerId != b.reviewerId
            ? a.reviewerId   < b.reviewerId
            : a.submissionId < b.submissionId;
    });

    // Always show what was assigned (even if partial)
    if (!sub.empty()) {
        out << "#SubmissionId,ReviewerId,Match\n";
        for (const auto& item : sub)
            out << item.submissionId << ", " << item.reviewerId << ", " << item.matchDomain << "\n";

        out << "#ReviewerId,SubmissionId,Match\n";
        for (const auto& item : rev)
            out << item.reviewerId << ", " << item.submissionId << ", " << item.matchDomain << "\n";

        out << "#Total: " << assignment.totalAssignments << "\n";
    }

    // Report missing reviews
    bool hasMissing = false;
    for (const Submission& s : data.submissions) {
        int count = 0;
        for (const auto& a : sub)
            if (a.submissionId == s.id) count++;
        int missing = data.parameters.minReviewsPerSubmission - count;
        if (missing > 0) {
            if (!hasMissing) { out << "#SubmissionId,Domain,MissingReviews\n"; hasMissing = true; }
            out << s.id << ", " << s.primaryDomain << ", " << missing << "\n";
        }
    }

    // Risk Analysis
    if (data.control.riskAnalysis == 1) {
        out << "#Risk Analysis: 1\n";
        vector<int> risky = riskAnalysisK1(data);
        for (int i = 0; i < (int)risky.size(); ++i) {
            if (i) out << ", ";
            out << risky[i];
        }
        if (risky.empty()) out << "\n";
        else out << "\n";
    }

    out.close();
}
