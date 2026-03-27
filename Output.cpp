#include "Output.h"
#include "Connection.h"
#include <fstream>
#include <algorithm>
using namespace std;

void printAssignment(ReviewAssignment assignment,AssignmentData data, string filename) {
    ofstream out(filename);
    auto sub = assignment.submissionAssignments;
    auto rev = assignment.reviewerAssignments;
    sort(sub.begin(), sub.end(), [](auto &a, auto &b) {
        if (a.submissionId != b.submissionId)
            return a.submissionId < b.submissionId;
        return a.reviewerId < b.reviewerId;
    });
    sort(rev.begin(), rev.end(), [](auto &a, auto &b) {
        if (a.reviewerId != b.reviewerId)
            return a.submissionId < b.submissionId;
        return a.reviewerId < b.reviewerId;
    });

    out << "#SubmissionId,ReviewerId,Match\n";
    for (auto item : sub)
        out << item.submissionId << ", " << item.reviewerId << ", " << item.matchDomain << "\n";

    out << "#ReviewerId,SubmissionId,Match\n";
    for (auto item : rev)
        out << item.submissionId << ", " << item.reviewerId << ", " << item.matchDomain << "\n";

    out << "#Total: " << assignment.totalAssignments << "\n";

    bool hasMissing = false;
    for (Submission s : data.submissions) {
        int count = 0;
        for (ReviewAssignmentItem a : sub) {
            if (a.submissionId == s.id)
                count++;
        }
        int missing = data.parameters.minReviewsPerSubmission - count;
        if (missing > 0) {
            if (!hasMissing) {
                out << "#SubmissionId,Domain,MissingReviews\n";
                hasMissing = true;
            }
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