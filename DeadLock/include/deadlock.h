#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include "structs.h"
using namespace std;

class DeadLock
{
public:
    // Global variable to store project name
    static string gProjectName;
    // Array containing all installed packages
    vector<string> installedPackages;
    // Starter function
    void init(string projectName);
    // File generators definiton
    void notebookGenerate(string projectName);
    void pyFileGenerate(string projectName);
    void gitignoreGenerate(string projectName);
    void readmeGenerate(string projectName);

    // Miscellanous
    void userOption();
    bool isPythonAvailable();
    string apiCaller(string url);

    // Package Downloaders
    bool downloadPackage(string packageName, string version);

    // Package installers
    bool installPackage(string package);
    bool installPackages(vector<string> packages);
    bool uninstallPackages(vector<string> packages);

    // Pypi package info retriever
    Package getPackageInfo(string packageName);
    string getLatestVersion(string packageName);

    // Wheel extraction
    bool extractWheelToVenv(string wheelPath, string venvPath);
    bool createVirtualEnvironment(string venvPath);

    // Dead.lock file management functions
    bool generateDeadLockFile(string projectPath = gProjectName);

    bool updateDeadLockFile(Package pkg);
    bool removeFromDeadLockFile(string packageName);

    // Dead.lock file operations
    bool isPackageInstalled(string packageName);
    bool syncFromDeadLock(string projectPath = gProjectName);

    // Dead.lock file utilities
    string getDeadLockFilePath();
    string getCurrentTimestamp();
    vector<string> getPackageDependencies(string packageName);
    vector<Package> getInstalledPackages();
    string readDeadLockFile(string filePath);

private:
    // Installation tracking
    vector<Package> loadedPackages;
    set<string> dependencyPackages; // Track packages that are dependencies

    // Helper functions for wheel extraction
    bool renameWheelToZip(string wheelPath, string zipPath);
    bool extractZipFile(string zipPath, string extractPath);
    bool parseAndExtractZip(vector<unsigned char> zipData, string extractPath);

    // Dead.lock file methods
    string generateDeadLockJson();
    bool isPkgInDeadLock(string packageName);
    bool writeDeadLockFile(string content, string filePath);
};
