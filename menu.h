/**
 * @file menu.h
 * @brief Interactive menu functions for the conference assignment tool.
 */

#ifndef PROJECT_DA_1_MENU_H
#define PROJECT_DA_1_MENU_H

#include "Data.h"

/**
 * @brief Displays the main menu.
 */
void showMainMenu();

/**
 * @brief Displays the data menu.
 */
void showDataMenu();

/**
 * @brief Runs the data menu.
 *
 * @param data Input problem data.
 */
void dataMenu(AssignmentData data);

/**
 * @brief Prints all submissions.
 *
 * @param data Input problem data.
 */
void printSubmissions(AssignmentData data);

/**
 * @brief Prints all reviewers.
 *
 * @param data Input problem data.
 */
void printReviewers(AssignmentData data);

/**
 * @brief Prints the problem parameters to the console.
 * @param data The currently loaded problem instance.
 */
void printParameters(AssignmentData data);

/**
 * @brief Prints the control settings.
 *
 * @param data Input problem data.
 */
void printControl(AssignmentData data);

/**
 * @brief Runs the main interactive menu.
 *
 * Prompts the user for a CSV file, then shows the main menu.
 * Loops until the user chooses to exit.
 *
 * @return 0 on normal exit.
 */
int mainMenu();

#endif