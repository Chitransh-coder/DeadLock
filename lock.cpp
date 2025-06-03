#include "deadlock.h"
#include <CLI/CLI.hpp>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    try {
        CLI::App app{"DeadLock CLI"};
        DeadLock dl;
        string project_name;
        CLI::App* create_cmd = app.add_subcommand("create", "Create a new project");
        create_cmd->add_option("name", project_name, "Project name")->required();
        
        CLI11_PARSE(app, argc, argv);
        
        if (*create_cmd) {
            dl.init(project_name);
        }
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
