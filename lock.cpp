#include "deadlock.h"
#include <CLI/CLI.hpp>
using namespace std;

int main(int argc, char** argv) {
    CLI::App app{"DeadLock CLI"};
    DeadLock dl;
    string project_name;
    CLI::App* create_cmd = app.add_subcommand("create", "Create a new project");
    create_cmd->add_option("name", project_name, "Project name")->required();
    
    CLI11_PARSE(app, argc, argv);
    
    if (*create_cmd) {
        dl.init(project_name);
    }
    
    return 0;
}
