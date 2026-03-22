#ifndef PROJECT_DA_1_MENU_H
#define PROJECT_DA_1_MENU_H

#include "Data.h"

void showMainMenu();
void showDataMenu();
void showAssignmentMenu();
void dataMenu(AssignmentData data);
void assignmentMenu(AssignmentData data);
void printSubmissions(AssignmentData data);
void printReviewers(AssignmentData data);
void printParameters(AssignmentData data);
void printControl(AssignmentData data);
int  mainMenu();

#endif
