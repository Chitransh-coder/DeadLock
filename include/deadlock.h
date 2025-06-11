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
    bool downloadPackage(const string& package);    // Pypi package info retriever
    string getPackageInfo(const string& packageName);
    string getLatestVersion(const string& packageName);
    
    // Wheel extraction and installation
    bool extractWheelToVenv(const string& wheelPath, const string& venvPath);
    bool createVirtualEnvironment(const string& venvPath);
    bool testPackageInstallation(const string& packageName, const string& venvPath);
    
private:
    // Helper functions for wheel extraction
    bool renameWheelToZip(const string& wheelPath, string& zipPath);
    bool extractZipFile(const string& zipPath, const string& extractPath);
    bool parseAndExtractZip(const vector<unsigned char>& zipData, const string& extractPath);
};
