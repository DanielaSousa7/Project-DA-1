/**
* @file menu.h
 * @brief Interactive menu functions for the conference assignment tool.
 */

#ifndef PROJECT_DA_1_MENU_H
#define PROJECT_DA_1_MENU_H

#include "Data.h"

/// @brief Displays the main menu options.
void showMainMenu();

/// @brief Displays the data browser submenu options.
void showDataMenu();

/// @brief Displays the assignment submenu options.
void showAssignmentMenu();

/**
 * @brief Runs the data browser submenu, allowing the user to inspect the loaded data.
 * @param data The currently loaded problem instance.
 */
void dataMenu(AssignmentData data);

/**
 * @brief Runs the assignment submenu, allowing the user to run and view assignments.
 * @param data The currently loaded problem instance.
 */
void assignmentMenu(AssignmentData data);

/**
 * @brief Prints all submissions to the console.
 * @param data The currently loaded problem instance.
 */
void printSubmissions(AssignmentData data);

/**
 * @brief Prints all reviewers to the console.
 * @param data The currently loaded problem instance.
 */
void printReviewers(AssignmentData data);

/**
 * @brief Prints the problem parameters to the console.
 * @param data The currently loaded problem instance.
 */
void printParameters(AssignmentData data);

/**
 * @brief Prints the control settings to the console.
 * @param data The currently loaded problem instance.
 */
void printControl(AssignmentData data);

/**
 * @brief Entry point for the interactive menu loop.
 *
 * Prompts the user for a CSV file, then shows the main menu.
 * Loops until the user chooses to exit.
 *
 * @return 0 on normal exit.
 */
int mainMenu();

#endif