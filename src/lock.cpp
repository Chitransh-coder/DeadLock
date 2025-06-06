#include "../include/deadlock.h"
#include "../CLI/CLI.hpp"
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
        }
        else if (*info_cmd) {
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
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
