# Programming Project I

This project aims to develop a tool to help with the assignment of reviewers to paper submissions.
Each paper belongs to a specific topic and each reviewer
has expertise in a certain area. The assignment must respect these areas of expertise,
while also ensuring that each submission is reviewed by a minimum number of reviewers and
that each reviewer does not exceed their review limit.

To solve this problem, the assignment is modeled as a network flow problem, allowing valid assignments
to be found. The project also considers risk analysis by studying how the
absence of reviewers may affect the review process.

We use the Edmonds-Karp maximum flow algorithm and did not make any alterations to the original graph data structure.


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


## T2.3 Basic Formulation with Risk Analysis = K


## T2.4 General Formulation without Risk Analysis


## Contributions 

This project was developed by:
- Daniela Sousa up202406448
- João Costa up202403747
- Rodrigo Correia up202405057

Class 12 - Group 5 