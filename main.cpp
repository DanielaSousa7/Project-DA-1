#include "menu.h"
#include "Batch.h"
#include "Connection.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        batchMode(argv[2], argv[3]);
        return 0;
    }
    return mainMenu();
}