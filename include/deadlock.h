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
    
    bool isPythonAvailable();
    // PyPI package management
    bool installPackage(const string& package, const string& version);
    bool installPackages(const vector<string>& packages);
    bool downloadPackage(const string& package);
    // Pypi package info retriever
    string getPackageInfo(const string& packageName);
    string getLatestVersion(const string& packageName);
};
