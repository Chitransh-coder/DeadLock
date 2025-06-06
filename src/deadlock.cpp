#include "../include/deadlock.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <../curl/curl.h>
#include <filesystem>
#include <sstream>
using namespace std;
namespace fs = std::filesystem;

// Callback function for writing data from curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

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
        readmeFile << "# " + projectName + "\n\n"
                  << "Add project description here.\n\n"
                  << "## Installation\n\n"
                  << "```bash\n"
                  << "pip install -e .\n"
                  << "```\n\n"
                  << "## Usage\n\n"
                  << "```python\n"
                  << "import " + projectName + "\n"
                  << "```";
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

bool DeadLock::installPackage(const string& packageName) {
    vector<string> packages = {packageName};
    return installPackages(packages);
}

bool DeadLock::installPackages(const vector<string>& packages) {
    if (packages.empty()) {
        cerr << "No packages specified for installation." << endl;
        return false;
    }

    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize libcurl." << endl;
        return false;
    }

    for (const auto& package : packages) {
        cout << "Fetching package information for " << package << "..." << endl;
        
        // Query PyPI for package info
        string url = "https://pypi.org/pypi/" + package + "/json";
        string response;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "DeadLock/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // For simplicity, disable SSL verification
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Failed to query PyPI: " << curl_easy_strerror(res) << endl;
            continue;
        }
        
        // Parse the response to get the download URL for the source distribution
        // Look for the source distribution (.tar.gz) in the URLs
        size_t urlsPos = response.find("\"urls\":");
        if (urlsPos == string::npos) {
            cerr << "No download URLs found for package: " << package << endl;
            continue;
        }
        
        size_t tarGzPos = response.find(".tar.gz", urlsPos);
        if (tarGzPos == string::npos) {
            cerr << "No source distribution found for package: " << package << endl;
            continue;
        }
        
        // Extract the download URL
        size_t urlStart = response.rfind("\"url\": \"", tarGzPos) + 8;
        size_t urlEnd = response.find("\"", urlStart);
        string downloadUrl = response.substr(urlStart, urlEnd - urlStart);
        
        // Download the source package
        cout << "Downloading source for " << package << "..." << endl;
        string sourceFile = package + ".tar.gz";
        
        // Download to file
        FILE* fp = fopen(sourceFile.c_str(), "wb");
        if (!fp) {
            cerr << "Failed to create file: " << sourceFile << endl;
            continue;
        }
        
        CURL* downloadCurl = curl_easy_init();
        curl_easy_setopt(downloadCurl, CURLOPT_URL, downloadUrl.c_str());
        curl_easy_setopt(downloadCurl, CURLOPT_WRITEFUNCTION, WriteToFile);
        curl_easy_setopt(downloadCurl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(downloadCurl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(downloadCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        
        CURLcode downloadRes = curl_easy_perform(downloadCurl);
        fclose(fp);
        curl_easy_cleanup(downloadCurl);
        
        if (downloadRes != CURLE_OK) {
            cerr << "Failed to download source: " << curl_easy_strerror(downloadRes) << endl;
            continue;
        }
        
        // Extract and build the package
        cout << "Extracting and building " << package << "..." << endl;
        
        // Extract tar.gz
        string extractCmd = "tar -xzf " + sourceFile;
        if (system(extractCmd.c_str()) != 0) {
            cerr << "Failed to extract " << sourceFile << endl;
            continue;
        }
        
        // Find the extracted directory 
        string buildDir = package + "-*";
        
        // Build the package using setup.py
        string buildCmd = "cd " + package + "-* && python setup.py build install";
        int buildResult = system(buildCmd.c_str());
        
        if (buildResult != 0) {
            cerr << "Failed to build package: " << package << endl;
            continue;
        }
        
        cout << "Successfully installed " << package << endl;
    }
    
    curl_easy_cleanup(curl);
    return true;
}

// Advanced PyPI functionality implementation

string DeadLock::getPackageInfo(const string& packageName) {
    CURL* curl = curl_easy_init();
    string response;
    
    if (!curl) {
        cerr << "Failed to initialize curl" << endl;
        return "";
    }
    
    string url = "https://pypi.org/pypi/" + packageName + "/json";
    
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
    // Simple JSON parsing - in production, consider using a proper JSON library like nlohmann/json
    size_t infoStart = info.find("\"info\":{");
    if (infoStart == string::npos) return "";
    
    // Find the version field within the info object
    size_t versionStart = info.find("\"version\":", infoStart);
    if (versionStart == string::npos) return "";
    
    // Skip to the actual value after the colon and any whitespace
    versionStart = info.find("\"", versionStart + 10);
    if (versionStart == string::npos) return "";
    versionStart++; // Skip the opening quote
    
    // Find the closing quote
    size_t versionEnd = info.find("\"", versionStart);
    if (versionEnd == string::npos) return "";
    
    string version = info.substr(versionStart, versionEnd - versionStart);
    return version;
    size_t infoPos = info.find("\"info\"");
    if (infoPos == string::npos) return "";
    
    size_t versionPos = info.find("\"version\":", infoPos);
    if (versionPos == string::npos) return "";
    
    size_t valueStart = info.find("\"", versionPos + 10) + 1;
    if (valueStart == string::npos) return "";
    
    size_t valueEnd = info.find("\"", valueStart);
    if (valueEnd == string::npos) return "";
    
    return info.substr(valueStart, valueEnd - valueStart);
}

// Function to download a file and save it to disk
size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

bool DeadLock::downloadPackage(const string& packageName, const string& version) {
    string packageInfo = getPackageInfo(packageName);
    if (packageInfo.empty()) {
        cerr << "Failed to get package information for " << packageName << endl;
        return false;
    }
    
    string targetVersion = version;
    if (version == "latest") {
        targetVersion = getLatestVersion(packageName);
        if (targetVersion.empty()) {
            cerr << "Failed to determine latest version for " << packageName << endl;
            return false;
        }
    }
    
    cout << "Downloading " << packageName << " version " << targetVersion << "..." << endl;
    
    // Create a download directory if it doesn't exist
    fs::create_directory("downloads");
    
    // In a full implementation, we would parse the JSON to get the download URL
    // Here, we'll construct a typical PyPI wheel URL for simplicity
    // In reality, you should extract the URL from the PyPI API response
    
    // For demonstration purposes, we'll construct a direct download URL to the wheel file
    // This won't work for all packages but serves as an example
    string downloadUrl = "https://files.pythonhosted.org/packages/source/" + 
                         packageName.substr(0, 1) + "/" + packageName + "/" +
                         packageName + "-" + targetVersion + ".tar.gz";
    
    string outputFile = "downloads/" + packageName + "-" + targetVersion + ".tar.gz";
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize curl" << endl;
        return false;
    }
    
    FILE* fp = fopen(outputFile.c_str(), "wb");
    if (!fp) {
        cerr << "Failed to create output file: " << outputFile << endl;
        curl_easy_cleanup(curl);
        return false;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "DeadLock/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    
    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    
    fclose(fp);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK || httpCode != 200) {
        cerr << "Failed to download package: " << curl_easy_strerror(res) 
             << ", HTTP code: " << httpCode << endl;
        
        // If direct download fails, use pip
        cout << "Falling back to pip installation..." << endl;
        string cmd = "pip download -d downloads " + packageName + "==" + targetVersion;
        int result = system(cmd.c_str());
        
        if (result != 0) {
            cerr << "Failed to download package with pip" << endl;
            return false;
        }
    }
    
    cout << "Successfully downloaded " << packageName << " to " << outputFile << endl;
    return true;
}
