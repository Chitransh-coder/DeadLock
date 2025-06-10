#include "../include/deadlock.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <../include/json.hpp>
#include <../curl/curl.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include <sstream>
using namespace std;
using json = nlohmann::json;

// Callback function for writing data from curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void DeadLock::init(string projectName) {
    if (!isPythonAvailable())
    {
        cerr << "Python not available! Please download latest version from https://www.python.org/" << endl;
        exit(1);
    }
// Check User OS
#ifdef _WIN32
    _mkdir(projectName.c_str());
#else
    mkdir(projectName.c_str(), 0755);
#endif
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
        json notebook = {
            {"cells", {{
            {"cell_type", "markdown"},
            {"metadata", json::object()},
            {"source", {"# Hello World"}}
            }}},
            {"metadata", {
            {"kernelspec", {
                {"display_name", "Python 3"},
                {"language", "python"},
                {"name", "python3"}
            }},
            {"language_info", {
                {"codemirror_mode", {
                {"name", "ipython"},
                {"version", 3}
                }},
                {"file_extension", ".py"},
                {"mimetype", "text/x-python"},
                {"name", "python"},
                {"nbconvert_exporter", "python"},
                {"pygments_lexer", "ipython3"},
                {"version", "3.11"}
            }},
            {"orig_nbformat", 4}
            }},
            {"nbformat", 4},
            {"nbformat_minor", 2}
        };
        
        noteBook << notebook.dump(4);
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
        readmeFile << "# " + projectName + "\n\n"
        "Add project description here.\n";
        readmeFile.close();
    }
    catch(const exception& e) {
        cerr << e.what() << endl;
    }
}

bool DeadLock::isPythonAvailable() {
    #ifdef _WIN32
    int result = system("where python >nul 2>nul");
    #else
    int result = system("which python >/dev/null 2>&1");
    #endif
    return result == 0;
}

// Initialize curl
bool DeadLock::installPackage(const string& package,const string& version) {
    string downloadUrl;
    bool foundCompatibleWheel = false;
    string url = "https://pypi.org/pypi/" + package + "/" + version + "/json";
    string response;
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize libcurl." << endl;
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "DeadLock/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);        
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        cerr << "Failed to query PyPI: " << curl_easy_strerror(res) << endl;
    }
    try {
        json j = json::parse(response);
        if (!j.contains("urls")) {
            throw exception("Cannot find valid url to download package");
        }
        if (!j["urls"].contains("filename"))
        {
            throw exception("Cannot find correct version to download for Windows");
        }
#ifdef _WIN32
        // Find Windows compatible wheel
        for (const auto& urlInfo : j["urls"]) {
            if (urlInfo.contains("filename") && urlInfo.contains("url")) {
                string fileName = urlInfo["filename"];
                if (fileName.find("win32.whl") != string::npos || 
                    fileName.find("win_amd64.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos) {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel) {
            throw exception("No Windows compatible wheel found for this package");
        }

#elif __APPLE__
        if (urlInfo.contains("filename") && urlInfo.contains("url")) {
                string fileName = urlInfo["filename"];
                if (fileName.find("macosx_") != string::npos || 
                    fileName.find("universal2.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos) {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel) {
            throw exception("No Windows compatible wheel found for this package");
        }
        
#else

        if (urlInfo.contains("filename") && urlInfo.contains("url")) {
                string fileName = urlInfo["filename"];
                if (fileName.find("linux_x86_64.whl") != string::npos || 
                    fileName.find("linux_aarch64.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos) {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel) {
            throw exception("No Windows compatible wheel found for this package");
        }

#endif
    } catch (json::exception& e) {
        cerr << "Error parsing JSON" << e.what() << endl;
        exit(1);
    } catch(exception& e) {
        cerr << "Error finding valid package" << e.what() << endl;
        exit(1);
    }
    return true;
}

bool DeadLock::installPackages(const vector<string>& packages) {
    if (packages.empty()) {
        cerr << "No packages specified for installation." << endl;
        return false;
    }

    for (const auto& package : packages) {
        string version = getLatestVersion(package);
        if (!installPackage(package, version))
        {
            cerr << "Package not installed: " << package << endl;
            return false;
        }
        cout << "Successfully installed " << package << endl;
    }
    return true;
}

string DeadLock::getPackageInfo(const string& packageName) {
    CURL* curl = curl_easy_init();
    string response;
    // Initialize curl
    if (!curl) {
        cerr << "Failed to initialize curl" << endl;
        return "";
    }
    // Set download URL
    string url = "https://pypi.org/pypi/" + packageName + "/json";
    // Set request to GET
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    // Set input URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // Callback to write data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    // Response
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    // User Agent Header
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "DeadLock/1.0");
    // Automatically follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Don't verify SSL, for now
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // Perform API request
    CURLcode res = curl_easy_perform(curl);
    // Clear memory
    curl_easy_cleanup(curl);
    // Check response
    if (res != CURLE_OK) {
        cerr << "Failed to query PyPI: " << curl_easy_strerror(res) << endl;
        return "";
    }
    return response;
}

string DeadLock::getLatestVersion(const string& packageName) {
    string info = getPackageInfo(packageName);
    if (info.empty()) {
        return "";
    } 
    // Parse JSON to extract version information
    try {
        json j = json::parse(info);
        if(j.contains("info")) {
            if (j["info"].contains("version"))
            {
                return j["info"]["version"];
            } else {
                throw exception("error retrieving version. Retry or report an issue on GitHub.");
            }
        } else if (j.contains("version")) {
            return j["version"];
        } else {
            throw exception("error retrieving version. Retry or report an issue on GitHub.");
        }
    } catch(json::exception& e) {
        cerr << "error parsing json: " << e.what() << endl;
    } catch (exception& e) {
        cerr << e.what() << endl;
    }
}

// Function to download a file and save it to disk
size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}
