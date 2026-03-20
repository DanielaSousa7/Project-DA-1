#ifndef PROJECT_DA_1_REVIEWASSIGNMENT_H
#define PROJECT_DA_1_REVIEWASSIGNMENT_H

#include <vector>

struct ReviewAssignmentItem {
    int submissionId;
    int reviewerId;
    int matchDomain;
};

struct ReviewAssignment {
    std::vector<ReviewAssignmentItem> submissionAssignments;
    std::vector<ReviewAssignmentItem> reviewerAssignments;
    int totalAssignments = 0;
};

#endif