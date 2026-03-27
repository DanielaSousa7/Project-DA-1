#ifndef PROJECT_DA_1_CONNECTION_H
#define PROJECT_DA_1_CONNECTION_H

#include "Data.h"
#include "ReviewAssignment.h"
#include <vector>
using namespace std;

int getMatchDomain(Submission s, Reviewer r, AssignmentData data);
ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId = -1);
bool assignmentFeasible(AssignmentData data, int excludeReviewerId = -1);
vector<int> riskAnalysis(AssignmentData data);

#endif