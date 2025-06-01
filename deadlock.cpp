#include "deadlock.h"
#include <fstream>
#include <iostream>
using namespace std;

int DeadLock::init(string projectName) {
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
        return 0;
    }
    catch(exception &e) {
        cerr << e.what() << '\n';
        return -1;
            }
        }