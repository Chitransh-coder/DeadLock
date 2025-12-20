#include "../include/deadlock.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    try
    {
        DeadLock dl = DeadLock();

        // Create command
        string project_name;
        // Install PyPI packages command
        vector<string> packages;
        // Sync from dead.lock file command
        // List installed packages command
        // Debug Mode (Test a function)
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
