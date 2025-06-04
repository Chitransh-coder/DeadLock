#include "../include/deadlock.h"
#include "CLI/CLI.hpp"
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
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
