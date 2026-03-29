/**
 * @file main.cpp
 * @brief Entry point for the Scientific Conference Assignment Tool.
 *
 * Supports two modes of operation:
 * - Interactive: launches a menu-driven interface
 * - Batch: `./myProg -b input.csv output.csv` runs non-interactively
 */

#include "menu.h"
#include "Batch.h"
#include "Connection.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * @brief Runs the program.
 *
 * Time complexity: depends on the selected mode.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @return 0 on normal execution.
 */

int main(int argc, char* argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        batchMode(argv[2], argv[3]);
        return 0;
    }
    return mainMenu();
}