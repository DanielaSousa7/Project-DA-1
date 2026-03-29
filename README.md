# Programming Project I

This project aims to develop a tool to help with the assignment of reviewers to paper submissions.
Each paper belongs to a specific topic and each reviewer
has expertise in a certain area. The assignment must respect these areas of expertise,
while also ensuring that each submission is reviewed by a minimum number of reviewers and
that each reviewer does not exceed their review limit.

To solve this problem, the assignment is modeled as a network flow problem, allowing valid assignments
to be found. The project also considers risk analysis by studying how the
absence of reviewers may affect the review process.

We use the Edmonds-Karp maximum flow algorithm and the only changes did is this algorihtm and in graph.h was add doxygen comments.


## Compilation and Execution of the Program 

### Compile using Makefile
- make clean  #optional
- make
### Run in Interactive Mode
- ./myProg

### Run in Batch Mode
- ./myProg -b input.csv output.csv 

where `input.csv` is the input file to be read and `output.csv` is the file where the results will be written.

## T2.2 Basic Formulation with Any Risk Analysis = 1

For the risk analysis variant, the goal is to determine whether the absence of a single reviewer makes a complete review assignment
impossible, meaning that some submissions do not receive the required number of reviews. This is implemented through the functions `assignmentFeasible` and `riskAnalysis` in `Connection.cpp`.

The function `assignmentFeasible` runs the assignment algorithm while excluding one reviewer from the flow network. It then compares the total number of assignments
found with the total number of assignments required, which is equal to the number of submissions multiplied by minReviewsPerSubmission.
If the maximum flow found is equal to that required value, the assignment is considered feasible.

The function `riskAnalysis` first checks whether the original instance is feasible when all reviewers are available. If it is already infeasible, then the analysis is not meaningful and an empty list is returned. Otherwise, it removes each reviewer one at a time by calling `assignmentFeasible(data, r.id)`,
where r.id is the identifier of the reviewer being excluded. If removing that reviewer makes the assignment infeasible, then that reviewer
is considered critical and its identifier is added to the result. At the end, the list of critical reviewers is sorted and returned.

Regarding complexity, one execution of the assignment algorithm has the complexity of Edmonds-Karp, which is O(V * E^2), where V is the number of
vertices and E is the number of edges in the flow network. In this code, the number of vertices is 2 + number of submissions + number of reviewers,
since the graph contains the source, sink, one node per submission and one node per reviewer. The number of edges includes one edge from the source
to each submission, one edge from each reviewer to the sink and in the worst case one edge from each submission to each reviewer, so it is proportional
to S + R + S*R, where S is the number of submissions and R is the number of reviewers. Therefore, a single run of the assignment algorithm has complexity O(V * E^2).
The time complexity of the risk analysis is O(R · V · E²), since Edmonds-Karp is executed once for each reviewer through calls to the function `assignmentFeasible`.

A nontrivial example where the absence of a single reviewer makes the assignment impossible is the following. Suppose minReviewsPerSubmission = 2 and maxReviewsPerReviewer = 2.

Consider three submissions:
- submission 1 has primary domain 1 and secondary domain 2;
- submission 2 has primary domain 1 and secondary domain 3;
- submission 3 has primary domain 2 and secondary domain 3.

Consider four reviewers:
- reviewer 10 has primary expertise 1;
- reviewer 11 has primary expertise 2;
- reviewer 12 has primary expertise 3;
- reviewer 13 has primary expertise 1.

If the code is running in mode 2, where a submission can match either its primary or secondary domain against the reviewer’s primary expertise, then:
- submission 1 can be assigned to reviewers 10, 11 and 13;
- submission 2 can be assigned to reviewers 10, 12 and 13;
- submission 3 can be assigned to reviewers 11 and 12.

With all reviewers available, a full assignment is possible. For example:
- submission 1 is assigned to reviewers 11 and 13;
- submission 2 is assigned to reviewers 10 and 12;
- submission 3 is assigned to reviewers 11 and 12.

Each submission receives exactly 2 reviews and no reviewer exceeds the maximum of 2 assigned reviews.

However, if reviewer 12 is removed, submission 3 can only be assigned to reviewer 11, since reviewer 12 was its only reviewer with expertise in domain 3. Therefore, submission 3 can no longer receive
the required 2 reviews. Therefore, the total number of assignments becomes smaller than the required value, and the instance becomes infeasible. This means reviewer 12 is critical according to the implemented risk analysis.

## T2.3 Basic Formulation with Risk Analysis = K

For the risk analysis variant with K > 1, the goal is to determine whether the simultaneous absence of any set of K reviewers makes a complete review assignment impossible, meaning that some submissions
do not receive the required number of reviews.

A natural extension of the approach used for K = 1 is to test every combination of K reviewers. First, the original instance is checked to verify whether it is feasible when all reviewers are available. If it
is already infeasible, then the analysis is not meaningful and no critical set is reported.

If the instance is feasible, for each subset of K reviewers, the flow network is rebuilt without those reviewers and the max-flow assignment algorithm is executed again. If the total number of assignments found is smaller than the required
number of assignments, that subset of reviewers is considered critical, since their simultaneous absence prevents a valid assignment.

This approach reuses the same idea of the Risk Analysis for K = 1. The only difference is that instead of excluding a single reviewer, it excludes a set of reviewers before running Edmonds-Karp.

The time complexity of this approach is dominated by the number of combinations of k reviewers that must be tested. Since all combinations of k reviewers
are considered, the total number of cases is C(R, K), which represents the number of ways to choose K reviewers from a total of R reviewers. For each subset of k reviewers, the Edmonds-Karp algorithm is executed, with complexity O(V * E^2). Therefore, the total
time complexity is O(C(R, K) * V * E^2).

A nontrivial example for the case K = 2 is the following. Suppose minReviewsPerSubmission = 2 and maxReviewsPerReviewer = 2.

Consider three submissions:

submission 1 has primary domain 1 and secondary domain 2;
submission 2 has primary domain 1 and secondary domain 3;
submission 3 has primary domain 2 and secondary domain 3.

Consider five reviewers:

reviewer 10 has primary expertise 1;
reviewer 11 has primary expertise 2;
reviewer 12 has primary expertise 3;
reviewer 13 has primary expertise 1;
reviewer 14 has primary expertise 2.

If the system is running in mode 2, where a submission may match either its primary or secondary domain against the reviewer’s primary expertise, then:

submission 1 can be assigned to reviewers 10, 11, 13 and 14;
submission 2 can be assigned to reviewers 10, 12 and 13;
submission 3 can be assigned to reviewers 11, 12 and 14.

With all reviewers available, a full assignment is possible, since each submission can receive 2 reviews and no reviewer exceeds the maximum of 2 assignments.

However, if reviewers 12 and 13 are both removed, submission 2 can only be assigned to reviewer 10, which is not enough to satisfy its minimum requirement of 2 reviews. Therefore,
the assignment becomes infeasible. This shows that the subset {12, 13} is a critical set of reviewers for K = 2.

Corresponding output:
Critical reviewer sets for K = 2:
{12, 13}


## T2.4 General Formulation without Risk Analysis

For the general formulation, the problem becomes more complex since both submissions and reviewers may have primary and secondary domains. As a result, different submission-reviewer
matches may have different levels of quality and not all valid assignments are equally desirable.

Instead of modifying the underlying Max-Flow algorithm, we extend the existing approach by introducing different levels of matching quality. The flow network structure remains the same: a source
node connects to submission nodes, which connect to reviewer nodes and finally to the sink. The capacities also remain unchanged, ensuring that each submission receives the required number of reviews and each reviewer does not exceed their maximum limit.

The key idea is to prioritize higher-quality matches. For example, a match between the primary domain of a submission and the primary expertise of a reviewer is considered better than matches involving
secondary domains. To incorporate this, matches are divided into levels of priority.

The assignment is then computed in multiple phases. In the first phase, the flow network is constructed using only the highest-quality matches and the Max-Flow algorithm is executed. If a complete
assignment is not found, additional edges corresponding to lower-quality matches are progressively added to the network and the algorithm is executed again. This process continues until either a complete assignment is found or all possible match types have been considered.

This approach ensures that higher-quality matches are always preferred, since lower-quality matches are only used when necessary. 

Regarding complexity, if L represents the number of matching levels, the Max-Flow algorithm is executed at most L times. Since each execution has complexity O(V * E^2), the overall complexity is O(L * V * E^2).

## Contributions 

This project was developed by:
- Daniela Sousa up202406448
- João Costa up202403747
- Rodrigo Correia up202405057

Class 12 - Group 5 