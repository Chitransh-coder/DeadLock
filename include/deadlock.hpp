#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include "structs.hpp"

class DeadLock
{
public:
    // Global variable to store project name
    static std::string gProjectName;
    // Array containing all installed packages
    std::vector<std::string> installedPackages;
    // Starter function
    void init(std::string projectName);
    // File generators definiton
    void notebookGenerate(std::string projectName);
    void pyFileGenerate(std::string projectName);
    void gitignoreGenerate(std::string projectName);
    void readmeGenerate(std::string projectName);

    // Miscellanous
    void userOption();
    bool isPythonAvailable();
    std::string apiCaller(std::string url);

    // Package Downloaders
    bool downloadPackage(std::string packageName, std::string version);

    // Package installers
    bool installPackage(std::string package);
    bool installPackages(std::vector<std::string> packages);
    bool uninstallPackages(std::vector<std::string> packages);

    // Pypi package info retriever
    Package getPackageInfo(std::string packageName);
    std::string getLatestVersion(std::string packageName);

    // Wheel extraction
    bool extractWheelToVenv(std::string wheelPath, std::string venvPath);
    bool createVirtualEnvironment(std::string venvPath);

    // Dead.lock file management functions
    bool generateDeadLockFile(std::string projectPath = gProjectName);

    bool updateDeadLockFile(Package pkg);
    bool removeFromDeadLockFile(std::string packageName);

    // Dead.lock file operations
    bool isPackageInstalled(std::string packageName);
    bool syncFromDeadLock(std::string projectPath = gProjectName);

    // Dead.lock file utilities
    std::string getDeadLockFilePath();
    std::string getCurrentTimestamp();
    std::vector<std::string> getPackageDependencies(std::string packageName);
    std::vector<Package> getInstalledPackages();
    std::string readDeadLockFile(std::string filePath);

private:
    // Installation tracking
    std::vector<Package> loadedPackages;
    std::set<std::string> dependencyPackages; // Track packages that are dependencies

    // Helper functions for wheel extraction
    bool renameWheelToZip(std::string wheelPath, std::string zipPath);
    bool extractZipFile(std::string zipPath, std::string extractPath);
    bool parseAndExtractZip(std::vector<unsigned char> zipData, std::string extractPath);

    // Dead.lock file methods
    void loadPackages();
    std::string generateDeadLockJson();
    bool isPkgInDeadLock(std::string packageName);
    bool writeDeadLockFile(std::string content, std::string filePath);
};
