#include "../include/deadlock.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    try {
        CLI::App app{"DeadLock CLI"};
        DeadLock dl = DeadLock();
        
        // Create command
        string project_name;
        CLI::App* create_cmd = app.add_subcommand("create", "Create a new project");
        create_cmd->add_option("name", project_name, "Project name")->required();
        
        // Install PyPI packages command
        CLI::App* install_cmd = app.add_subcommand("install", "Install Python packages from PyPI");

        vector<string> packages;
        install_cmd->add_option("packages", packages, "Package names to install")->required()->expected(1, -1);

        // Sync from dead.lock file command
        CLI::App* sync_cmd = app.add_subcommand("sync", "Install packages from dead.lock file");
        
        // List installed packages command
        CLI::App* list_cmd = app.add_subcommand("list", "List all installed packages from dead.lock file");
        // Debug Mode (Test a function)
        CLI::App* test_cmd = app.add_subcommand("test", "test a specific function");
        string testArg;
        test_cmd->add_option("testArgument", testArg, "testing function argument");

          CLI11_PARSE(app, argc, argv);
        
        if (*create_cmd) {
            dl.init(project_name);
        } 
        else if (*install_cmd) {
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
        } else if (*sync_cmd) {
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
                vector<Package> packages = dl.getInstalledPackages();
                
                if (packages.empty()) {
                    cout << "No packages found in dead.lock file." << endl;
                } else {
                    cout << "\nInstalled packages:" << endl;
                    cout << "===================" << endl;
                    
                    for (const auto& pkg : packages) {
                        cout << pkg.name << " @ " << pkg.version;
                        cout << "\n  Source: " << pkg.source << endl;
                        cout << "  Install Date: " << pkg.installDate << endl;
                        // if (!pkg.dependencies.empty()) {
                        //     cout << "  Dependencies: ";
                        //     for (size_t i = 0; i < pkg.dependencies.size(); ++i) {
                        //         cout << pkg.dependencies[i];
                        //         if (i < pkg.dependencies.size() - 1) cout << ", ";
                        //     }
                        //     cout << endl;
                        // }
                        cout << endl;
                    }
                }
            } else {
                cerr << "Failed to load dead.lock file." << endl;
                return 1;
            }
        } else if (*test_cmd) {
            // This command is used to test different functions.
            string response = dl.getPackageInfo(testArg);
            cout << response << endl;
        }
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
