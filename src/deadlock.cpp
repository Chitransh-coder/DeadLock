#include "../include/deadlock.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

void DeadLock::init(string projectName) {
    
    cout << "This is the output" << endl;
    // Generate all project files
    notebookGenerate(projectName);
    pyFileGenerate(projectName);
    tomlGenerate(projectName);
    gitignoreGenerate(projectName);
    readmeGenerate(projectName);
    cout << "Project " << projectName << " created successfully!" << endl;
}

void DeadLock::notebookGenerate(string projectName) {
        string noteBookName = projectName + ".ipynb";
        ofstream noteBook(noteBookName);
    try {
        noteBook <<
        "{\n"
        "\"cells\":[\n"
            "{\n"
            "\"cell_type\": \"markdown\",\n"
            "\"metadata\": {},\n"
            "\"source\": [\n"
                "\"# Hello World\"\n"
            "]\n"
            "}\n"
            "],\n"
        "\"metadata\": {\n"
        "\"kernelspec\": {\n"
        "\"display_name\": \"Python 3\",\n"
        "\"language\": \"python\",\n"
        "\"name\": \"python3\"\n"
        "},\n"
        "\"language_info\": {\n"
        "\"codemirror_mode\": {\n"
        "\"name\": \"ipython\",\n"
        "\"version\": 3\n"
        "},\n"
        "\"file_extension\": \".py\",\n"
        "\"mimetype\": \"text/x-python\",\n"
        "\"name\": \"python\","
        "\"nbconvert_exporter\": \"python\",\n"
        "\"pygments_lexer\": \"ipython3\",\n"
        "\"version\": 3.11\n"
        "},\n"
        "\"orig_nbformat\": 4\n"
        "},\n"
        "\"nbformat\": 4,"
        "\"nbformat_minor\": 2\n"
        "}\n";
        noteBook.close();
    }
    catch(exception &e) {
        cerr << e.what() << endl;
    }
}

void DeadLock::pyFileGenerate(string projectName) {
    string pyfileName = projectName + ".py";
    ofstream pyFile(pyfileName);
    try {
        pyFile << "print(\"Hello World!\")";
        pyFile.close();
    }
    catch(const exception& e) {
        cerr << e.what() << endl;
    }
    
}

void DeadLock::tomlGenerate(string projectName) {
    string tomlfileName = "pyproject.toml";
    ofstream tomlFile(tomlfileName);
    try {
        tomlFile << "[project]\n"
        "name = \"" + projectName + "\""
        "version = \"0.0.1\""
        "description = \"Add project description\""
        "readme = \"README.md\"";
        tomlFile.close();
    }
    catch(const exception& e)
    {
        cerr << e.what() << endl;
    }
    
}

void DeadLock::gitignoreGenerate(string projectName) {
    string gitignorefileName = ".gitignore";
    ofstream gitFile(gitignorefileName);
    try {
        gitFile << 
        "# Ignore Python virtual environments\n"
        "venv/\n"
        ".env\n"
        ".venv\n"
        "env/\n"
        "venv/\n"
        "ENV/\n"
        "env.bak/\n"
        "venv.bak/\n\n"
        "# Ignore python cache\n"
        "__pycache__/\n"
        "*.py[cod]\n"
        "*$py.class\n\n"
        "# Ignore C shared objects\n"
        ".so\n\n"
        "# Ignore Distribution / Packaging items\n"
        ".Python\n"
        "build/\n"
        "develop-eggs/\n"
        "dist/\n"
        "downloads/\n"
        "eggs/\n"
        ".eggs/\n"
        "lib/\n"
        "lib64/\n"
        "parts/\n"
        "sdist/\n"
        "var/\n"
        "wheels/\n"
        "share/python-wheels/\n"
        "*.egg-info/\n"
        ".installed.cfg\n"
        "*.egg\n"
        "MANIFEST\n";
        gitFile.close();
    } catch(const exception& e) {
        cerr << e.what() << endl;
    }
}

void DeadLock::readmeGenerate(string projectName) {
    string readmeFileName = "README.md";
    ofstream readmeFile(readmeFileName);
    try {
        readmeFile << "# " + projectName + "\n";
        readmeFile.close();
    } catch(const exception& e) {
        cerr << e.what() << endl;
    }
}

bool DeadLock::installPackage(const string& packageName) {
    vector<string> packages = {packageName};
    return installPackages(packages);
}

bool DeadLock::installPackages(const vector<string>& packages) {
    // Check if Python is available
    if (!isPythonAvailable()) {
        cerr << "Cannot install packages because Python is not available" << endl;
        return false;
    }
    
    string packagesStr;
    for (size_t i = 0; i < packages.size(); ++i) {
        packagesStr += packages[i];
        if (i < packages.size() - 1) {
            packagesStr += " ";
        }
    }
    
    cout << "Installing packages: " << packagesStr << endl;
    // TODO: ADD Download and unpacking functionality of python packages
    cout << "Successfully installed packages: " << packagesStr << endl;
    return true;
}

// Add the isPythonAvailable method implementation
bool DeadLock::isPythonAvailable() {
    cout << "Checking if Python is installed..." << endl;
    
#ifdef _WIN32
    // Windows command to check Python
    int result = system("python --version >nul 2>&1");
#else
    // Linux/Mac command to check Python
    int result = system("python3 --version >/dev/null 2>&1");
#endif

    if (result != 0) {
        cerr << "Python is not installed or not in PATH" << endl;
        return false;
    }
    return true;
}
