/**
 * @file Output.cpp
 * @brief Implementation of the function that writes the assignment result to a CSV file.
 */

#include "Output.h"
#include "Connection.h"
#include <fstream>
#include <algorithm>
using namespace std;

/**
 * @brief Writes the assignment result to a CSV file
 *
 * Sorts submissions by paper ID then reviewer ID, and reviewers by reviewer ID
 * then paper ID. Also reports any papers that did not receive enough reviews,
 * and runs the risk analysis if requested in the control settings.
 *
 * Time complexity: O(A log A + R * A + K), where A is the number of assignments,
 * R is the number of submissions and K is the cost of the risk analysis.
 *
 * @param assignment The result of running the assignment algorithm.
 * @param data       The full problem instance.
 * @param filename   Path to the output CSV file.
 */
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
        out << "#Risk Analysis: 1\n";
        vector<int> risky = riskAnalysis(data);
        for (size_t i = 0; i < risky.size(); ++i) {
            if (i) out << ", ";
            out << risky[i];
        }
        out << "\n";
    }
    out.close();
}