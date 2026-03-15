#include "Application.h"
#include "Menu.h"
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    // Batch mode: ./myProg -b input.csv
    if (argc >= 3 && string(argv[1]) == "-b")
        return Application::runBatch(argv[2]);

    // Interactive mode
    Application app;
    runInteractiveMenu(app);
    return 0;
}