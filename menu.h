#ifndef PROJECT_DA_1_MENU_H
#define PROJECT_DA_1_MENU_H

#include "data.h"

void showMenu();
void printSubmissions(const AssignmentData &data);
void printReviewers(const AssignmentData &data);
void printParameters(const AssignmentData &data);
void printControl(const AssignmentData &data);
int mainMenu();

#endif //PROJECT_DA_1_MENU_H