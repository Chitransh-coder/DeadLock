#include "../include/deadlock.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    try {
        CLI::App app{"DeadLock CLI"};
        DeadLock dl;
        
        // Create command
        string project_name;
        CLI::App* create_cmd = app.add_subcommand("create", "Create a new project");
        create_cmd->add_option("name", project_name, "Project name")->required();
        
        // Install PyPI packages command
        CLI::App* pip_cmd = app.add_subcommand("pip", "Install Python packages from PyPI");
        vector<string> packages;
        pip_cmd->add_option("packages", packages, "Package names to install")->required()->expected(1, -1);
        
        // Download PyPI packages command
        CLI::App* download_cmd = app.add_subcommand("download", "Download Python packages from PyPI without installing");
        vector<string> download_packages;
        download_cmd->add_option("packages", download_packages, "Package names to download")->required()->expected(1, -1);
        // Get package info command
        CLI::App* info_cmd = app.add_subcommand("info", "Get information about a Python package from PyPI");
        string info_package;
        info_cmd->add_option("package", info_package, "Package name to get information about")->required();
        
        // Generate dead.lock file command
        CLI::App* lock_cmd = app.add_subcommand("lock", "Generate or update dead.lock file");
        
        // Sync from dead.lock file command
        CLI::App* sync_cmd = app.add_subcommand("sync", "Install packages from dead.lock file");
        
        // List installed packages command
        CLI::App* list_cmd = app.add_subcommand("list", "List all installed packages from dead.lock file");
        
        // Validate dead.lock file command
        CLI::App* validate_cmd = app.add_subcommand("validate", "Validate dead.lock file structure");
          CLI11_PARSE(app, argc, argv);
        
        if (*create_cmd) {
            dl.init(project_name);
        } 
        else if (*pip_cmd) {
            if (!dl.isPythonAvailable()) {
                cerr << "Python is not available. Please install Python and add it to your PATH." << endl;
                return 1;
            }
            
            cout << "Installing packages from PyPI: ";
            for (const auto& pkg : packages) {
                cout << pkg << " ";
            }
            cout << endl;
            
            if (dl.installPackages(packages)) {
                cout << "Successfully installed packages!" << endl;
            } else {
                cerr << "Failed to install packages." << endl;
                return 1;
            }
        }
        else if (*download_cmd) {
            cout << "Downloading packages from PyPI: ";
            for (const auto& pkg : download_packages) {
                cout << pkg << " ";
            }
            cout << endl;
            
            bool success = true;
            for (const auto& pkg : download_packages) {
                if (!dl.downloadPackage(pkg)) {
                    cerr << "Failed to download package: " << pkg << endl;
                    success = false;
                }
            }
            
            if (success) {
                cout << "Successfully downloaded all packages!" << endl;
            } else {
                cerr << "Failed to download some packages." << endl;
                return 1;
            }
        }        else if (*info_cmd) {
            cout << "Getting information for package: " << info_package << endl;
            
            string version = dl.getLatestVersion(info_package);
            if (!version.empty()) {
                cout << "Package: " << info_package << endl;
                cout << "Latest version: " << version << endl;
                cout << "PyPI URL: https://pypi.org/project/" << info_package << "/" << endl;
            } else {
                cerr << "Failed to get information for package: " << info_package << endl;
                return 1;
            }
        }
        else if (*lock_cmd) {
            cout << "Generating/updating dead.lock file..." << endl;
            
            if (dl.generateDeadLockFile(".")) {
                cout << "Successfully generated dead.lock file!" << endl;
            } else {
                cerr << "Failed to generate dead.lock file." << endl;
                return 1;
            }
        }
        else if (*sync_cmd) {
            cout << "Syncing packages from dead.lock file..." << endl;
            
            if (dl.syncFromDeadLock(".")) {
                cout << "Successfully synced all packages!" << endl;
            } else {
                cerr << "Failed to sync packages from dead.lock file." << endl;
                return 1;
            }
        }
        else if (*list_cmd) {
            cout << "Loading packages from dead.lock file..." << endl;
            
            if (dl.loadDeadLockFile(".")) {
                vector<PackageDependency> packages = dl.getInstalledPackages();
                
                if (packages.empty()) {
                    cout << "No packages found in dead.lock file." << endl;
                } else {
                    cout << "\nInstalled packages:" << endl;
                    cout << "===================" << endl;
                    
                    for (const auto& pkg : packages) {
                        cout << pkg.name << " @ " << pkg.version;
                        if (pkg.isDev) {
                            cout << " (dev)";
                        }
                        cout << "\n  Source: " << pkg.source << endl;
                        cout << "  Install Date: " << pkg.installDate << endl;
                        if (!pkg.dependencies.empty()) {
                            cout << "  Dependencies: ";
                            for (size_t i = 0; i < pkg.dependencies.size(); ++i) {
                                cout << pkg.dependencies[i];
                                if (i < pkg.dependencies.size() - 1) cout << ", ";
                            }
                            cout << endl;
                        }
                        cout << endl;
                    }
                }
            } else {
                cerr << "Failed to load dead.lock file." << endl;
                return 1;
            }
        }
        else if (*validate_cmd) {
            cout << "Validating dead.lock file..." << endl;
            
            if (dl.validateDeadLockFile(".")) {
                cout << "dead.lock file is valid!" << endl;
            } else {
                cerr << "dead.lock file validation failed." << endl;
                return 1;
            }
        }
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
