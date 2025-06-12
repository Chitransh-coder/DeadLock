#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

// Structure to represent a package dependency
struct PackageDependency {
    string name;
    string version;
    string source; // "pypi", "local", etc.
    string installDate;
    vector<string> dependencies; // Sub-dependencies
    string hash; // Package hash for integrity
    bool isDev; // Development dependency flag
};

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
    
    // Wheel extraction and installation
    bool extractWheelToVenv(const string& wheelPath, const string& venvPath);
    bool createVirtualEnvironment(const string& venvPath);
    bool testPackageInstallation(const string& packageName, const string& venvPath);
    
    // Dead.lock file management functions
    bool generateDeadLockFile(const string& projectPath = ".");
    bool loadDeadLockFile(const string& projectPath = ".");
    bool updateDeadLockFile(const string& packageName, const string& version, 
                           const string& source = "pypi", bool isDev = false);
    bool removeFromDeadLockFile(const string& packageName);
    bool validateDeadLockFile(const string& projectPath = ".");
    
    // Dead.lock file operations
    vector<PackageDependency> getInstalledPackages() const;
    PackageDependency getPackageDependency(const string& packageName) const;
    bool isPackageInstalled(const string& packageName) const;
    bool syncFromDeadLock(const string& projectPath = ".");
    
    // Dead.lock file utilities
    string getDeadLockFilePath(const string& projectPath = ".") const;
    string getCurrentTimestamp() const;
    string calculatePackageHash(const string& packageName, const string& version) const;
    vector<string> getPackageDependencies(const string& packageName, const string& version) const;
    
private:
    // Helper functions for wheel extraction
    bool renameWheelToZip(const string& wheelPath, string& zipPath);
    bool extractZipFile(const string& zipPath, const string& extractPath);
    bool parseAndExtractZip(const vector<unsigned char>& zipData, const string& extractPath);
    
    // Private dead.lock file helpers
    map<string, PackageDependency> installedPackages;
    bool parseDeadLockJson(const string& jsonContent);
    string generateDeadLockJson() const;
    bool writeDeadLockFile(const string& content, const string& filePath) const;
    string readDeadLockFile(const string& filePath) const;
    bool backupDeadLockFile(const string& filePath) const;
    bool validatePackageEntry(const PackageDependency& package) const;
};
