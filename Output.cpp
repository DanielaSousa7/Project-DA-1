#include "Output.h"
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

void ReviewOutput::printAssignment(const ReviewAssignment &assignment, const std::string &filename) {
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

    out << "#SubmissionId,ReviewerId,Match\n";
    for (auto &item : sub)
        out << item.submissionId << "," << item.reviewerId << "," << item.matchDomain << "\n";

    out << "#ReviewerId,SubmissionId,Match\n";
    for (auto &item : rev)
        out << item.reviewerId << "," << item.submissionId << "," << item.matchDomain << "\n";

    out << "#Total: " << assignment.totalAssignments << "\n";

    out.close();
}