#ifndef PROJECT_DA_1_CONNECTION_H
#define PROJECT_DA_1_CONNECTION_H

#include "Data.h"
#include "ReviewAssignment.h"
#include <vector>
using namespace std;

// Runs the review assignment. excludeReviewerId = -1 means include all reviewers.
ReviewAssignment runReviewAssignment(AssignmentData data, int excludeReviewerId = -1);

// Returns true if a full valid assignment is possible (with optional excluded reviewer).
bool assignmentFeasible(AssignmentData data, int excludeReviewerId = -1);

// Risk Analysis K=1: returns IDs of reviewers whose absence breaks the assignment.
vector<int> riskAnalysisK1(AssignmentData data);

#endif