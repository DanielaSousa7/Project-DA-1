#ifndef PROJECT_DA_1_OUTPUT_H
#define PROJECT_DA_1_OUTPUT_H

#include "Data.h"
#include "Connection.h"
#include "ReviewAssignment.h"
#include <string>

class ReviewOutput {
public:
    static void printAssignment(const ReviewAssignment &assignment, const std::string &filename);
    static void printUnassignedSubmissions(const AssignmentData &data, const ReviewAssignment &assignment);
};


#endif //PROJECT_DA_1_OUTPUT_H