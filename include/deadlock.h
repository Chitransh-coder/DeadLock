#pragma once
#include <string>
#include <vector>
using namespace std;

class DeadLock {
public:
    // Starter function
    void init(string projectName);
    // File generators definiton
    void notebookGenerate(string projectName);
    void pyFileGenerate(string projectName);
    void tomlGenerate(string projectName);
    void gitignoreGenerate(string projectName);
    void readmeGenerate(string projectName);
    // PyPI package management
    bool installPackage(const string& packageName);
    bool installPackages(const vector<string>& packages);
    bool isPythonAvailable();
};
