/**
* @file batch.h
 * @brief Provides batch processing functionality for the project.
 *
 * This header declares the batchMode function, which is used to process
 * input and output files in batch mode.
 */

#ifndef PROJECT_DA_1_BATCH_H
#define PROJECT_DA_1_BATCH_H

#include <string>
using namespace std;

/**
 * @brief Executes batch processing mode.
 *
 * Reads data from the specified input file, processes it in batch mode,
 * and writes the results to the output file.
 *
 * @param inputFile  Path to the input file containing batch commands or data.
 * @param outputFile Path to the output file where results will be written.
 *
 * @note This function is intended to be called from the main program
 *       when batch mode is selected.
 */

void batchMode(string inputFile, string outputFile);

#endif //PROJECT_DA_1_BATCH_H