#include "Output.h"
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

    if (data.control.riskAnalysis == 1) {
        out << "#Risk Analysis: " << data.control.riskAnalysis << "\n";
        vector<int> risky;
        for ( Reviewer r: data.reviewers) {
            int count = 0;
            for (ReviewAssignmentItem a : rev) {
                if (a.reviewerId == r.id)
                    count++;
            }
            if (count > data.parameters.maxReviewsPerReviewer) {
                risky.push_back(r.id);
            }
        }
        sort(risky.begin(), risky.end());
        for (size_t i = 0; i < risky.size(); i++) {
            if (i > 0) out << ", ";
            out << risky[i];
        }
        out << "\n";
    }
    out.close();
}