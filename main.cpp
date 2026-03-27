/**
* @file main.cpp
 * @brief Entry point for the Scientific Conference Assignment Tool.
 *
 * Supports two modes of operation:
 * - Interactive: launches a menu-driven interface
 * - Batch: `./proj -b input.csv [output.csv]` runs non-interactively
 */

#include "menu.h"
#include "Batch.h"
#include "Connection.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * @brief Program entry point.
 *
 * Usage:
 * - `./proj`                     — interactive menu
 * - `./proj -b input.csv`        — batch mode, output to file specified in CSV
 * - `./proj -b input.csv out.csv` — batch mode, output to out.csv
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Main menu.
 */

int main(int argc, char* argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        batchMode(argv[2], argv[3]);
        return 0;
    }
    return mainMenu();
}