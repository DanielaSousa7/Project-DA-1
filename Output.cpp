#include "Output.h"
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

void ReviewOutput::printAssignment(const ReviewAssignment &assignment,
                                   const AssignmentData &data,
                                   const std::string &filename) {

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
            return a.reviewerId < b.reviewerId;
        return a.submissionId < b.submissionId;
    });

    // ---------------- ASSIGNMENTS ----------------
    out << "#SubmissionId,ReviewerId,Match\n";
    for (auto &item : sub)
        out << item.submissionId << ", " << item.reviewerId << ", " << item.matchDomain << "\n";

    out << "#ReviewerId,SubmissionId,Match\n";
    for (auto &item : rev)
        out << item.reviewerId << ", " << item.submissionId << ", " << item.matchDomain << "\n";

    out << "#Total: " << assignment.totalAssignments << "\n";

    // ---------------- MISSING REVIEWS ----------------
    out << "#SubmissionId,Domain,MissingReviews\n";

    bool allSatisfied = true;

    for (const auto &s : data.submissions) {

        int count = 0;

        for (const auto &a : sub) {
            if (a.submissionId == s.id)
                count++;
        }

        int missing = data.parameters.minReviewsPerSubmission - count;
        if (missing < 0) missing = 0;

        if (missing > 0)
            allSatisfied = false;

        out << s.id << ", " << s.primaryDomain << ", " << missing << "\n";
    }

    // ---------------- RISK ANALYSIS ----------------
    if (data.control.riskAnalysis != 0) {

        out << "#Risk Analysis: " << data.control.riskAnalysis << "\n";

        vector<int> risky;

        for (const auto &r : data.reviewers) {

            int count = 0;

            for (const auto &a : rev) {
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