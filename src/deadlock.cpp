#include "../include/deadlock.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include "../include/json.hpp"
#include "../curl/curl.h"
#include <zlib.h>
#include <sstream>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif
using namespace std;
using json = nlohmann::json;

size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream);

// Callback function for writing data from curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// ZIP file structures
#pragma pack(push, 1)
struct ZipLocalFileHeader {
    uint32_t signature;          // 0x04034b50
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
};

struct ZipCentralDirectoryHeader {
    uint32_t signature;          // 0x02014b50
    uint16_t versionMadeBy;
    uint16_t versionNeeded;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
    uint16_t commentLength;
    uint16_t diskNumber;
    uint16_t internalAttributes;
    uint32_t externalAttributes;
    uint32_t relativeOffset;
};

struct ZipEndOfCentralDirectory {
    uint32_t signature;          // 0x06054b50
    uint16_t diskNumber;
    uint16_t centralDirDisk;
    uint16_t numEntriesThisDisk;
    uint16_t numEntries;
    uint32_t centralDirSize;
    uint32_t centralDirOffset;
    uint16_t commentLength;
};
#pragma pack(pop)

// Helper functions for ZIP parsing
uint16_t readUint16(const unsigned char* data, size_t offset) {
    return data[offset] | (data[offset + 1] << 8);
}

uint32_t readUint32(const unsigned char* data, size_t offset) {
    return data[offset] | (data[offset + 1] << 8) | 
           (data[offset + 2] << 16) | (data[offset + 3] << 24);
}

bool createDirectoryRecursive(const string& path) {
    size_t pos = 0;
    string dir;
    
    if (path[path.length() - 1] != '/' && path[path.length() - 1] != '\\') {
        // Remove filename from path
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash == string::npos) {
            return true; // No directory to create
        }
        dir = path.substr(0, lastSlash);
    } else {
        dir = path;
    }
    
    while ((pos = dir.find_first_of("/\\", pos)) != string::npos) {
        string subdir = dir.substr(0, pos++);
        if (!subdir.empty()) {
#ifdef _WIN32
            _mkdir(subdir.c_str());
#else
            mkdir(subdir.c_str(), 0755);
#endif
        }
    }
    
    if (!dir.empty()) {
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }
    
    return true;
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
    }    try {
        json j = json::parse(response);
        if (!j.contains("urls")) {
            throw exception("Cannot find valid url to download package");
        }
        
        // Check if urls array is not empty
        if (j["urls"].empty()) {
            throw exception("No download URLs available for this package");
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
        for (const auto& urlInfo : j["urls"]) {
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
            throw exception("No macOS compatible wheel found for this package");
        }
        
#else
        for (const auto& urlInfo : j["urls"]) {
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
            throw exception("No Linux compatible wheel found for this package");
        }
#endif

        // Create downloads directory
#ifdef _WIN32
        _mkdir("downloads");
#else
        mkdir("downloads", 0755);
#endif

        // Extract filename from URL
        string wheelFileName = downloadUrl.substr(downloadUrl.find_last_of('/') + 1);
        string outputPath = "downloads/" + wheelFileName;

        // Download the wheel file
        cout << "Downloading " << wheelFileName << "..." << endl;
        
        FILE* fp = fopen(outputPath.c_str(), "wb");
        if (!fp) {
            cerr << "Failed to create output file: " << outputPath << endl;
            return false;
        }

        CURL* downloadCurl = curl_easy_init();
        if (!downloadCurl) {
            cerr << "Failed to initialize curl for download" << endl;
            fclose(fp);
            return false;
        }

        curl_easy_setopt(downloadCurl, CURLOPT_URL, downloadUrl.c_str());
        curl_easy_setopt(downloadCurl, CURLOPT_WRITEFUNCTION, WriteToFile);
        curl_easy_setopt(downloadCurl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(downloadCurl, CURLOPT_USERAGENT, "DeadLock/1.0");
        curl_easy_setopt(downloadCurl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(downloadCurl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode downloadRes = curl_easy_perform(downloadCurl);
        long httpCode = 0;
        curl_easy_getinfo(downloadCurl, CURLINFO_RESPONSE_CODE, &httpCode);        fclose(fp);
        curl_easy_cleanup(downloadCurl);
        
        if (downloadRes != CURLE_OK || httpCode != 200) {
            cerr << "Failed to download wheel: " << curl_easy_strerror(downloadRes) 
                 << ", HTTP code: " << httpCode << endl;
            return false;
        }

        cout << "Download completed successfully!" << endl;
        
        // Create virtual environment if it doesn't exist
        string venvPath = "venv";
        if (!createVirtualEnvironment(venvPath)) {
            cerr << "Failed to create virtual environment" << endl;
            return false;
        }
        
        // Extract wheel to virtual environment
        if (!extractWheelToVenv(outputPath, venvPath)) {
            cerr << "Failed to extract wheel to virtual environment" << endl;
            return false;
        }
        
        // Test if package is properly installed
        if (!testPackageInstallation(package, venvPath)) {
            cerr << "Package installation verification failed" << endl;
            return false;
        }
        
        cout << "Package " << package << " successfully installed and verified!" << endl;

    } catch (json::exception& e) {
        cerr << "Error parsing JSON" << e.what() << endl;
        return false;
    } catch(exception& e) {
        cerr << "Error finding valid package" << e.what() << endl;
        return false;
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

bool DeadLock::downloadPackage(const string& package) {
    string version = getLatestVersion(package);
    if (version.empty()) {
        cerr << "Failed to get latest version for package: " << package << endl;
        return false;
    }
    return installPackage(package, version);
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
    }    return ""; // Default return for error cases
}

bool DeadLock::createVirtualEnvironment(const string& venvPath) {
    // Check if virtual environment already exists
    string sitePackagesPath;
#ifdef _WIN32
    sitePackagesPath = venvPath + "\\Lib\\site-packages";
    if (_access(sitePackagesPath.c_str(), 0) == 0) {
        cout << "Virtual environment already exists at: " << venvPath << endl;
        return true;
    }
    
    // Create virtual environment using Python
    string command = "python -m venv " + venvPath;
    cout << "Creating virtual environment: " << venvPath << endl;
    int result = system(command.c_str());
    
    if (result != 0) {
        cerr << "Failed to create virtual environment" << endl;
        return false;
    }
    
    // Verify creation
    return _access(sitePackagesPath.c_str(), 0) == 0;
#else
    sitePackagesPath = venvPath + "/lib/python*/site-packages";
    if (access((venvPath + "/bin/python").c_str(), F_OK) == 0) {
        cout << "Virtual environment already exists at: " << venvPath << endl;
        return true;
    }
    
    string command = "python3 -m venv " + venvPath;
    cout << "Creating virtual environment: " << venvPath << endl;
    int result = system(command.c_str());
    
    if (result != 0) {
        cerr << "Failed to create virtual environment" << endl;
        return false;
    }
    
    return access((venvPath + "/bin/python").c_str(), F_OK) == 0;
#endif
}

bool DeadLock::renameWheelToZip(const string& wheelPath, string& zipPath) {
    // Generate zip file path
    zipPath = wheelPath;
    size_t dotPos = zipPath.find_last_of('.');
    if (dotPos != string::npos) {
        zipPath = zipPath.substr(0, dotPos) + ".zip";
    } else {
        zipPath += ".zip";
    }
    
    cout << "Renaming wheel file to zip: " << wheelPath << " -> " << zipPath << endl;
    
    // Rename the file
#ifdef _WIN32
    if (MoveFileA(wheelPath.c_str(), zipPath.c_str()) == 0) {
        cerr << "Failed to rename wheel file to zip" << endl;
        return false;
    }
#else
    if (rename(wheelPath.c_str(), zipPath.c_str()) != 0) {
        cerr << "Failed to rename wheel file to zip" << endl;
        return false;
    }
#endif
    
    return true;
}

bool DeadLock::extractZipFile(const string& zipPath, const string& extractPath) {
    cout << "Extracting zip file using zlib: " << zipPath << " to " << extractPath << endl;
    
    FILE* file = fopen(zipPath.c_str(), "rb");
    if (!file) {
        cerr << "Failed to open zip file: " << zipPath << endl;
        return false;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize <= 0) {
        cerr << "Invalid file size" << endl;
        fclose(file);
        return false;
    }
    
    // Read entire file into memory
    vector<unsigned char> zipData(fileSize);
    size_t bytesRead = fread(zipData.data(), 1, fileSize, file);
    fclose(file);
    
    if (bytesRead != static_cast<size_t>(fileSize)) {
        cerr << "Failed to read zip file completely" << endl;
        return false;
    }
    
    // Create extraction directory
#ifdef _WIN32
    _mkdir(extractPath.c_str());
#else
    mkdir(extractPath.c_str(), 0755);
#endif
    
    if (zipData.size() < 22) {
        cerr << "File too small to be a valid ZIP" << endl;
        return false;
    }
    
    // Check for ZIP file signature (PK)
    if (zipData[0] != 'P' || zipData[1] != 'K') {
        cerr << "Not a valid ZIP file" << endl;
        return false;
    }
    
    // Parse ZIP file using zlib
    if (!parseAndExtractZip(zipData, extractPath)) {
        cerr << "Failed to extract ZIP file using zlib" << endl;
        return false;
    }
    
    cout << "ZIP extraction completed successfully using zlib" << endl;
    return true;
}

bool DeadLock::extractWheelToVenv(const string& wheelPath, const string& venvPath) {
    // Rename wheel to zip
    string zipPath;
    if (!renameWheelToZip(wheelPath, zipPath)) {
        return false;
    }
    
    // Create temporary extraction directory
    string tempExtractPath = "temp_extract";
#ifdef _WIN32
    _mkdir(tempExtractPath.c_str());
#else
    mkdir(tempExtractPath.c_str(), 0755);
#endif
    
    // Extract zip file
    if (!extractZipFile(zipPath, tempExtractPath)) {
        return false;
    }
    
    // Find site-packages directory in venv
    string sitePackagesPath;
#ifdef _WIN32
    sitePackagesPath = venvPath + "\\Lib\\site-packages";
#else
    // Find the python version directory
    string libPath = venvPath + "/lib";
    DIR* dir = opendir(libPath.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string dirName = entry->d_name;
            if (dirName.find("python") == 0 && dirName != "." && dirName != "..") {
                sitePackagesPath = libPath + "/" + dirName + "/site-packages";
                break;
            }
        }
        closedir(dir);
    }
    
    if (sitePackagesPath.empty()) {
        sitePackagesPath = venvPath + "/lib/python3/site-packages";
    }
#endif
    
    // Create site-packages directory if it doesn't exist
#ifdef _WIN32
    _mkdir(sitePackagesPath.c_str());
#else
    mkdir(sitePackagesPath.c_str(), 0755);
#endif
    
    // Copy extracted contents to site-packages
    cout << "Copying package contents to site-packages..." << endl;
    
#ifdef _WIN32
    string copyCommand = "xcopy \"" + tempExtractPath + "\" \"" + sitePackagesPath + "\" /E /I /Y";
#else
    string copyCommand = "cp -r \"" + tempExtractPath + "\"/* \"" + sitePackagesPath + "\"/";
#endif
    
    int result = system(copyCommand.c_str());
    if (result != 0) {
        cerr << "Failed to copy package contents to site-packages" << endl;
        return false;
    }
    
    // Clean up temporary files
#ifdef _WIN32
    system(("rmdir /S /Q \"" + tempExtractPath + "\"").c_str());
    _unlink(zipPath.c_str());
#else
    system(("rm -rf \"" + tempExtractPath + "\"").c_str());
    unlink(zipPath.c_str());
#endif
    
    cout << "Package successfully extracted to virtual environment!" << endl;
    return true;
}

bool DeadLock::testPackageInstallation(const string& packageName, const string& venvPath) {
    cout << "Testing package installation: " << packageName << endl;
    
    // Test by trying to import the package in the virtual environment
#ifdef _WIN32
    string pythonPath = venvPath + "\\Scripts\\python.exe";
    string testCommand = pythonPath + " -c \"import " + packageName + "; print('Package " + packageName + " imported successfully')\"";
#else
    string pythonPath = venvPath + "/bin/python";
    string testCommand = pythonPath + " -c \"import " + packageName + "; print('Package " + packageName + " imported successfully')\"";
#endif
    
    cout << "Running test command: " << testCommand << endl;
    int result = system(testCommand.c_str());
    
    if (result == 0) {
        cout << "Package installation verification successful!" << endl;
        return true;
    } else {
        cerr << "Package installation verification failed. The package might not be properly installed or importable." << endl;
        return false;
    }
}

// Function to download a file and save it to disk
size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

bool DeadLock::parseAndExtractZip(const vector<unsigned char>& zipData, const string& extractPath) {
    cout << "Parsing ZIP file with zlib decompression..." << endl;
    
    // Find End of Central Directory Record (EOCD)
    size_t zipSize = zipData.size();
    size_t eocdOffset = zipSize - 22; // Start from minimum EOCD size
    
    // Search for EOCD signature backwards from end of file
    bool foundEOCD = false;
    for (size_t i = eocdOffset; i > 0; i--) {
        if (i + 22 <= zipSize && readUint32(zipData.data(), i) == 0x06054b50) {
            eocdOffset = i;
            foundEOCD = true;
            break;
        }
    }
    
    if (!foundEOCD) {
        cerr << "Could not find End of Central Directory record" << endl;
        return false;
    }
    
    // Parse EOCD
    uint16_t numEntries = readUint16(zipData.data(), eocdOffset + 10);
    uint32_t centralDirSize = readUint32(zipData.data(), eocdOffset + 12);
    uint32_t centralDirOffset = readUint32(zipData.data(), eocdOffset + 16);
    
    cout << "Found " << numEntries << " entries in ZIP file" << endl;
    
    if (centralDirOffset + centralDirSize > zipSize) {
        cerr << "Invalid central directory offset or size" << endl;
        return false;
    }
    
    // Process each entry in the central directory
    size_t currentOffset = centralDirOffset;
    for (uint16_t i = 0; i < numEntries; i++) {
        if (currentOffset + 46 > zipSize) {
            cerr << "Invalid central directory entry offset" << endl;
            return false;
        }
        
        // Check central directory header signature
        if (readUint32(zipData.data(), currentOffset) != 0x02014b50) {
            cerr << "Invalid central directory header signature" << endl;
            return false;
        }
        
        // Parse central directory header
        uint16_t compressionMethod = readUint16(zipData.data(), currentOffset + 10);
        uint32_t compressedSize = readUint32(zipData.data(), currentOffset + 20);
        uint32_t uncompressedSize = readUint32(zipData.data(), currentOffset + 24);
        uint16_t filenameLength = readUint16(zipData.data(), currentOffset + 28);
        uint16_t extraFieldLength = readUint16(zipData.data(), currentOffset + 30);
        uint16_t commentLength = readUint16(zipData.data(), currentOffset + 32);
        uint32_t localHeaderOffset = readUint32(zipData.data(), currentOffset + 42);
        
        // Extract filename
        if (currentOffset + 46 + filenameLength > zipSize) {
            cerr << "Invalid filename length" << endl;
            return false;
        }
        
        string filename(reinterpret_cast<const char*>(zipData.data() + currentOffset + 46), filenameLength);
        cout << "Processing file: " << filename << " (compressed: " << compressedSize << ", uncompressed: " << uncompressedSize << ")" << endl;
        
        // Skip directories
        if (filename.back() == '/' || filename.back() == '\\') {
            cout << "Skipping directory: " << filename << endl;
            currentOffset += 46 + filenameLength + extraFieldLength + commentLength;
            continue;
        }
        
        // Find local header
        if (localHeaderOffset + 30 > zipSize) {
            cerr << "Invalid local header offset" << endl;
            return false;
        }
        
        // Check local header signature
        if (readUint32(zipData.data(), localHeaderOffset) != 0x04034b50) {
            cerr << "Invalid local header signature" << endl;
            return false;
        }
        
        // Parse local header to get filename and extra field lengths
        uint16_t localFilenameLength = readUint16(zipData.data(), localHeaderOffset + 26);
        uint16_t localExtraFieldLength = readUint16(zipData.data(), localHeaderOffset + 28);
        
        // Calculate data offset
        size_t dataOffset = localHeaderOffset + 30 + localFilenameLength + localExtraFieldLength;
        
        if (dataOffset + compressedSize > zipSize) {
            cerr << "Invalid data offset or compressed size" << endl;
            return false;
        }
        
        // Create directory structure for the file
        string fullPath = extractPath + "/" + filename;
        if (!createDirectoryRecursive(fullPath)) {
            cerr << "Failed to create directory for: " << fullPath << endl;
            return false;
        }
          // Extract and decompress file data
        vector<unsigned char> decompressedData(max(uncompressedSize, 1u)); // Ensure at least 1 byte for empty files
        
        if (compressionMethod == 0) {
            // No compression (stored)
            if (compressedSize != uncompressedSize) {
                cerr << "Size mismatch for stored file: " << filename << endl;
                return false;
            }
            if (compressedSize > 0) {
                memcpy(decompressedData.data(), zipData.data() + dataOffset, compressedSize);
            }
        } else if (compressionMethod == 8) {
            // Deflate compression
            if (uncompressedSize == 0) {
                // Handle empty files
                cout << "Skipping decompression of empty file: " << filename << endl;
            } else {
                z_stream strm = {};
                strm.next_in = const_cast<unsigned char*>(zipData.data() + dataOffset);
                strm.avail_in = compressedSize;
                strm.next_out = decompressedData.data();
                strm.avail_out = uncompressedSize;
                
                // Initialize for raw deflate (no zlib header/trailer)
                int ret = inflateInit2(&strm, -MAX_WBITS);
                if (ret != Z_OK) {
                    cerr << "Failed to initialize zlib inflation for file: " << filename << endl;
                    return false;
                }
                
                ret = inflate(&strm, Z_FINISH);
                inflateEnd(&strm);
                
                if (ret != Z_STREAM_END || strm.total_out != uncompressedSize) {
                    cerr << "Failed to decompress file: " << filename << " (error: " << ret << ")" << endl;
                    return false;
                }
            }
        } else {
            cerr << "Unsupported compression method: " << compressionMethod << " for file: " << filename << endl;
            return false;
        }
        
        // Write decompressed data to file
        ofstream outFile(fullPath, ios::binary);
        if (!outFile) {
            cerr << "Failed to create output file: " << fullPath << endl;
            return false;
        }
        
        outFile.write(reinterpret_cast<const char*>(decompressedData.data()), uncompressedSize);
        outFile.close();
        
        if (!outFile.good()) {
            cerr << "Failed to write file: " << fullPath << endl;
            return false;
        }
        
        cout << "Successfully extracted: " << fullPath << endl;
        
        // Move to next central directory entry
        currentOffset += 46 + filenameLength + extraFieldLength + commentLength;
    }
    
    cout << "All entries processed successfully" << endl;
    return true;
}
