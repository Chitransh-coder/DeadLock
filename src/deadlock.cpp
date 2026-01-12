#include "./include/deadlock.h"
#include "./include/structs.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <zlib.h>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif
using json = nlohmann::json;

string DeadLock::gProjectName = ".";

// Function to write data to a file and save it to disk
size_t WriteToFile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

// Callback function for writing data from curl
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *userp)
{
    userp->append((char *)contents, size * nmemb);
    return size * nmemb;
}

uint16_t readUint16(const unsigned char *data, size_t offset)
{
    return data[offset] | (data[offset + 1] << 8);
}

uint32_t readUint32(const unsigned char *data, size_t offset)
{
    return data[offset] | (data[offset + 1] << 8) |
           (data[offset + 2] << 16) | (data[offset + 3] << 24);
}

bool createDirectoryRecursive(const string &path)
{
    size_t pos = 0;
    string dir;

    if (path[path.length() - 1] != '/' && path[path.length() - 1] != '\\')
    {
        // Remove filename from path
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash == string::npos)
        {
            return true; // No directory to create
        }
        dir = path.substr(0, lastSlash);
    }
    else
    {
        dir = path;
    }

    while ((pos = dir.find_first_of("/\\", pos)) != string::npos)
    {
        string subdir = dir.substr(0, pos++);
        if (!subdir.empty())
        {
#ifdef _WIN32
            _mkdir(subdir.c_str());
#else
            mkdir(subdir.c_str(), 0755);
#endif
        }
    }

    if (!dir.empty())
    {
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }

    return true;
}

/**
 * @brief Perform an HTTP Get request to the given URL Using cURL.
 *
 *This function initializes a cURL session sends the request, collect the response and return the server's reply as the string.
 *
 * @param url The target URL to which the HTTP GET request will be send.
 *
 * @returns string The response body from the server, or an empty string if the request fails.
 */
string DeadLock::apiCaller(string url)
{
    CURL *curl = curl_easy_init();
    string response;
    if (!curl)
    {
        cerr << "Error initializing cURL object" << endl;
        return NULL;
    }
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
    if (res != CURLE_OK)
    {
        cerr << "Failed to query PyPI: " << curl_easy_strerror(res) << endl;
        return NULL;
    }
    return response;
}

/**
 * @brief Initializes the environment and creates project
 *
 * This function gets invoked when `create` argument is passed in CLI.
 * @param projectName The name of the project, the name specified here should not change in the future.
 */
void DeadLock::init(string projectName)
{
    gProjectName = projectName;
    int type = 0, option = 0;
    vector<string> packages(10);
    if (!isPythonAvailable())
    {
        cerr << "Python not available! Please download latest version from https://www.python.org/" << endl;
        exit(1);
    }
// Check User OS
#ifdef _WIN32
    _mkdir(projectName.c_str());
    createVirtualEnvironment(".venv");
#else
    mkdir(projectName.c_str(), 0755);
    createVirtualEnvironment(".venv");
#endif
    userOption();
    // Generate all project files
    notebookGenerate(projectName);
    pyFileGenerate(projectName);
    gitignoreGenerate(projectName);
    readmeGenerate(projectName);
    cout << "Project " << projectName << " created successfully!" << endl;
}

/**
 * @brief Generate the Python notebook.
 *
 * @param projectName The name of the project.
 */
void DeadLock::notebookGenerate(string projectName)
{
    string noteBookName = projectName + ".ipynb";
    ofstream noteBook(noteBookName);
    try
    {
        json notebook = {
            {"cells", {{{"cell_type", "markdown"}, {"metadata", json::object()}, {"source", {"# Hello World"}}}}},
            {"metadata", {{"kernelspec", {{"display_name", "Python 3"}, {"language", "python"}, {"name", "python3"}}}, {"language_info", {{"codemirror_mode", {{"name", "ipython"}, {"version", 3}}}, {"file_extension", ".py"}, {"mimetype", "text/x-python"}, {"name", "python"}, {"nbconvert_exporter", "python"}, {"pygments_lexer", "ipython3"}, {"version", "3.11"}}}, {"orig_nbformat", 4}}},
            {"nbformat", 4},
            {"nbformat_minor", 2}};

        noteBook << notebook.dump(4);
        noteBook.close();
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
}

/**
 * @brief Generate the Python File.
 *
 * @param projectName The name of the project.
 */
void DeadLock::pyFileGenerate(string projectName)
{
    string pyfileName = projectName + ".py";
    ofstream pyFile(pyfileName);
    try
    {
        pyFile << "print(\"Hello World!\")";
        pyFile.close();
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
}

/**
 * @brief Generate the .gitignore.
 *
 * @param projectName The name of the project.
 */
void DeadLock::gitignoreGenerate(string projectName)
{
    string gitignorefileName = ".gitignore";
    ofstream gitFile(gitignorefileName);
    try
    {
        gitFile << "# Ignore Python virtual environments\n"
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
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
}

/**
 * @brief Generate the README.md.
 *
 * @param projectName The name of the project.
 */
void DeadLock::readmeGenerate(string projectName)
{
    string readmeFileName = "README.md";
    ofstream readmeFile(readmeFileName);
    try
    {
        readmeFile << "# " + projectName + "\n\n"
                                           "Add project description here.\n";
        readmeFile.close();
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
}

/**
 * @brief Checks if Python is installed and accessible in the system's PATH.
 *
 * @returns `True` if Python is available in the PATH.
 */
bool DeadLock::isPythonAvailable()
{
#ifdef _WIN32
    int result = system("where python >nul 2>nul");
#else
    int result = system("which python >/dev/null 2>&1");
#endif
    return result == 0;
}

/**
 * @brief Downloads a platform-compatible Python package wheel from PyPI.
 *
 * This function retrieves package metadata from the PyPI JSON API for the given package name
 * and version, determines the appropriate wheel file for the current operating system
 * (Windows, macOS, or Linux), and downloads it to the local "downloads" directory.
 * If the version is not provided, the latest available version is fetched automatically.
 *
 * @param packageName The name of the Python package to download.
 * @param version The version of the package to download. If empty, the latest version is used.
 * @returns `True` if the package is successfully downloaded.
 */
bool DeadLock::downloadPackage(string packageName, string version)
{
    if (version.empty())
    {
        version = getLatestVersion(packageName);
    }
    string downloadUrl;
    bool foundCompatibleWheel = false;
    string apiURL = "https://pypi.org/pypi/" + packageName + "/" + version + "/json";
    string serverReply = apiCaller(apiURL);
    try
    {
        json jsonResponse = json::parse(serverReply);
        if (!jsonResponse.contains("info"))
        {
            throw runtime_error("Cannot retrieve info of the package.");
        }

        if (!jsonResponse.contains("urls"))
        {
            throw runtime_error("Cannot find valid url to download package");
        }

        // Check if urls array is not empty
        if (jsonResponse["urls"].empty())
        {
            throw runtime_error("No download URLs available for this package");
        }
#ifdef _WIN32
        // Find Windows compatible wheel
        for (auto &urlInfo : jsonResponse["urls"])
        {
            if (urlInfo.contains("filename") && urlInfo.contains("url"))
            {
                string fileName = urlInfo["filename"];
                if (fileName.find("win32.whl") != string::npos ||
                    fileName.find("win_amd64.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos)
                {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel)
        {
            throw runtime_error("No Windows compatible wheel found for this package");
        }

#elif __APPLE__
        // Look for MacOS compatible wheel
        for (auto &urlInfo : jsonResponse["urls"])
        {
            if (urlInfo.contains("filename") && urlInfo.contains("url"))
            {
                string fileName = urlInfo["filename"];
                if (fileName.find("macosx_") != string::npos ||
                    fileName.find("universal2.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos)
                {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel)
        {
            throw runtime_error("No macOS compatible wheel found for this package");
        }

#else
        // Look for Linux Compatible wheel
        for (auto &urlInfo : jsonResponse["urls"])
        {
            if (urlInfo.contains("filename") && urlInfo.contains("url"))
            {
                string fileName = urlInfo["filename"];
                if (fileName.find("linux_x86_64.whl") != string::npos ||
                    fileName.find("linux_aarch64.whl") != string::npos ||
                    fileName.find("py3-none-any.whl") != string::npos ||
                    fileName.find("py2.py3-none-any.whl") != string::npos)
                {
                    downloadUrl = urlInfo["url"];
                    foundCompatibleWheel = true;
                    break;
                }
            }
        }
        if (!foundCompatibleWheel)
        {
            throw runtime_error("No Linux compatible wheel found for this package");
        }
#endif
        // Extract filename from URL
        string wheelFileName = downloadUrl.substr(downloadUrl.find_last_of('/') + 1);
        string outputPath = "downloads/" + wheelFileName;

        // Create downloads directory
#ifdef _WIN32
        _mkdir("downloads");
#else
        mkdir("downloads", 0755);
#endif

        FILE *fp = fopen(outputPath.c_str(), "wb");
        if (!fp)
        {
            cerr << "Failed to create output file: " << outputPath << endl;
            return false;
        }

        CURL *downloadCurl = curl_easy_init();
        if (!downloadCurl)
        {
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
        cout << "Downloading package " << wheelFileName << endl;
        CURLcode downloadRes = curl_easy_perform(downloadCurl);
        long httpCode = 0;
        curl_easy_getinfo(downloadCurl, CURLINFO_RESPONSE_CODE, &httpCode);
        fclose(fp);
        curl_easy_cleanup(downloadCurl);

        if (downloadRes != CURLE_OK || httpCode != 200)
        {
            cerr << "Failed to download wheel: " << curl_easy_strerror(downloadRes) << ", HTTP code: " << httpCode << endl;
            return false;
        }
        return true;
    }
    catch (json::exception &e)
    {
        cerr << "Failed to parse json response from the server." << endl;
        return false;
    }
    catch (exception &e)
    {
        cerr << "Failed to download package " << packageName << endl;
        return false;
    }
}

/**
 * @brief Installs the package in `site-packages` directory
 *
 * @param package String containing name of the package
 * @returns `True` if the package was successfully installed.
 */
bool DeadLock::installPackage(string package)
{
    ifstream fileExists(getDeadLockFilePath());
    if (fileExists)
    {
        cout << "dead.lock file found, updating file" << endl;
    }
    else
    {
        cout << "dead.lock file not found, generating a new one" << endl;
        bool succ = generateDeadLockFile(getDeadLockFilePath());
        if (!succ)
        {
            cout << "Error generating new dead.lock file" << endl;
        }
    }
    fileExists.close();
    if (isPackageInstalled(package))
    {
        cout << "Package already installed" << endl;
        return true;
    }
    string deadLockStr = readDeadLockFile(getDeadLockFilePath());
    try
    {
        json deadLockJSON = json::parse(deadLockStr);
        if (deadLockJSON.contains("packages"))
        {
            Package pkg = getPackageInfo(package);
            if (pkg.empty())
            {
                cerr << "Error finding information about " << package << endl;
            }
            pkg.installDate = getCurrentTimestamp();
            updateDeadLockFile(pkg);
        }
        else
        {
        }
    }
    catch (json::exception &e)
    {
        cerr << "Error parsing JSON" << e.what() << endl;
    }
    return true;
}

bool DeadLock::installPackages(vector<string> packages)
{
    if (packages.empty())
    {
        cerr << "No packages specified for installation." << endl;
        return false;
    }

    // Clear any existing state
    installedPackages.clear();
    dependencyPackages.clear();

    // Add all packages to the installedPackages array
    for (auto &package : packages)
    {
        installedPackages.push_back(package);
    }

    // Process packages until all are installed
    while (!installedPackages.empty())
    {
        string currentPackage = installedPackages.front();
        installedPackages.erase(installedPackages.begin());

        cout << "Processing package: " << currentPackage << endl;

        // Check if this package is a dependency (already being handled)
        bool isDependency = dependencyPackages.find(currentPackage) != dependencyPackages.end();

        if (!isDependency)
        {
            // Get latest version
            string version = getLatestVersion(currentPackage);
            if (version.empty())
            {
                cerr << "Could not get version for package: " << currentPackage << endl;
                return false;
            }

            // Get dependencies for this package
            vector<string> dependencies = getPackageDependencies(currentPackage);

            if (!dependencies.empty())
            {
                cout << "Found " << dependencies.size() << " dependencies for " << currentPackage << ":" << endl;

                // Add dependencies to the front of the installedPackages array
                // and mark them as dependency packages
                for (auto &dep : dependencies)
                {
                    cout << "  - " << dep << endl;
                    installedPackages.insert(installedPackages.begin(), dep);
                    dependencyPackages.insert(dep);
                }
            }
        }

        // Install the current package
        string version = getLatestVersion(currentPackage);
        if (version.empty())
        {
            cerr << "Could not get version for package: " << currentPackage << endl;
            return false;
        }

        if (!installPackage(currentPackage))
        {
            cerr << "Package not installed: " << currentPackage << endl;
            return false;
        }

        cout << "Successfully installed " << currentPackage << endl;
    }

    return true;
}

/**
 * @brief Retrieves package metadata from the PyPI JSON API.
 *
 * This function sends an HTTP GET request to the PyPI JSON API for the specified package,
 * parses the response to extract key package information including name, latest version,
 * source designation, and dependencies.
 *
 * @param packageName The name of the package to query on PyPI.
 * @returns `Package` An object containing package details.
 */
Package DeadLock::getPackageInfo(string packageName)
{
    Package pkg;
    string url = "https://pypi.org/pypi/" + packageName + "/json";
    string response = apiCaller(url);
    try
    {
        json jsonResponse = json::parse(response);
        pkg.name = packageName;
        pkg.version = getLatestVersion(packageName);
        pkg.source = "pypi"; // Will include way to install from other sources in future
        pkg.dependencies = getPackageDependencies(packageName);
    }
    catch (json::exception &e)
    {
        cerr << "Failed to parse API response " << endl;
        return pkg;
    }
    catch (exception &e)
    {
        cerr << "Error retrieving information for package" << packageName << endl;
        return pkg;
    }
    return pkg;
}

/**
 * @brief Retrieves the latest version of a package from PyPI.
 *
 * This function queries the PyPI JSON API for the specified package and extracts the latest version
 * from the response.
 *
 * @param packageName The name of the package to query on PyPI.
 * @returns The latest version of the package, or an empty string if not found.
 */
string DeadLock::getLatestVersion(string packageName)
{
    string url = "https://pypi.org/pypi/" + packageName + "/json";
    string info = apiCaller(url);
    if (info.empty())
    {
        return "";
    }
    trx
    {
        json j = json::parse(info);
        if (j.contains("info"))
        {
            if (j["info"].contains("version"))
            {
                return j["info"]["version"];
            }
            else
            {
                throw runtime_error("error retrieving version. Retry or report an issue on GitHub.");
            }
        }
        else if (j.contains("version"))
        {
            return j["version"];
        }
        else
        {
            throw runtime_error("error retrieving version. Retry or report an issue on GitHub.");
        }
    }
    catch (json::exception &e)
    {
        cerr << "error parsing json: " << e.what() << endl;
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
    return "";
}

/**
 * @brief Creates a virtual environment at the specified path.
 *
 * This function checks if a virtual environment already exists at the given path,
 * and if not, it creates a new virtual environment using Python's venv module.
 *
 * @param venvPath The path where the virtual environment should be created.
 * @returns `True` if the virtual environment was successfully created or already exists.
 */
bool DeadLock::createVirtualEnvironment(string venvPath)
{
    // Check if virtual environment already exists
    string sitePackagesPath;
#ifdef _WIN32
    sitePackagesPath = venvPath + "\\Lib\\site-packages";
    if (_access(sitePackagesPath.c_str(), 0) == 0)
    {
        cout << "Virtual environment already exists at: " << venvPath << endl;
        return true;
    }

    // Create virtual environment using Python
    string command = "python -m venv " + venvPath;
    cout << "Creating virtual environment... " << endl;
    int result = system(command.c_str());

    if (result != 0)
    {
        cerr << "Failed to create virtual environment" << endl;
        return false;
    }

    // Verify creation
    return _access(sitePackagesPath.c_str(), 0) == 0;
#else
    sitePackagesPath = venvPath + "/lib/python*/site-packages";
    if (access((venvPath + "/bin/python").c_str(), F_OK) == 0)
    {
        cout << "Virtual environment already exists at: " << venvPath << endl;
        return true;
    }

    string command = "python3 -m venv " + venvPath;
    cout << "Creating virtual environment: " << venvPath << endl;
    int result = system(command.c_str());

    if (result != 0)
    {
        cerr << "Failed to create virtual environment" << endl;
        return false;
    }

    return access((venvPath + "/bin/python").c_str(), F_OK) == 0;
#endif
}

/**
 * @brief Renames a wheel file to a zip file.
 *
 * This function renames the specified wheel file to a zip file by changing its extension.
 *
 * @param wheelPath The path to the wheel file to be renamed.
 * @param zipPath The path where the renamed zip file will be saved.
 * @returns `True` if the renaming was successful.
 */
bool DeadLock::renameWheelToZip(string wheelPath, string zipPath)
{
    // Generate zip file path
    zipPath = wheelPath;
    size_t dotPos = zipPath.find_last_of('.');
    if (dotPos != string::npos)
    {
        zipPath = zipPath.substr(0, dotPos) + ".zip";
    }
    else
    {
        zipPath += ".zip";
    }

    cout << "Renaming wheel file to zip: " << wheelPath << " -> " << zipPath << endl;

    // Rename the file
#ifdef _WIN32
    if (MoveFileA(wheelPath.c_str(), zipPath.c_str()) == 0)
    {
        cerr << "Failed to rename wheel file to zip" << endl;
        return false;
    }
#else
    if (rename(wheelPath.c_str(), zipPath.c_str()) != 0)
    {
        cerr << "Failed to rename wheel file to zip" << endl;
        return false;
    }
#endif

    return true;
}

/**
 * @brief Extracts a ZIP file to the specified directory using zlib.
 * This function reads the ZIP file into memory, parses its contents,
 * and extracts the files to the specified directory.
 *
 * @param zipPath The path to the ZIP file to be extracted.
 * @param extractPath The directory where the contents of the ZIP file will be extracted.
 *
 * @returns `True` if the extraction was successful.
 */
bool DeadLock::extractZipFile(string zipPath, string extractPath)
{
    FILE *file = fopen(zipPath.c_str(), "rb");
    if (!file)
    {
        cerr << "Failed to open zip file: " << zipPath << endl;
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize <= 0)
    {
        cerr << "Invalid file size" << endl;
        fclose(file);
        return false;
    }

    // Read entire file into memory
    vector<unsigned char> zipData(fileSize);
    size_t bytesRead = fread(zipData.data(), 1, fileSize, file);
    fclose(file);

    if (bytesRead != static_cast<size_t>(fileSize))
    {
        cerr << "Failed to read zip file completely" << endl;
        return false;
    }

    // Create extraction directory
#ifdef _WIN32
    _mkdir(extractPath.c_str());
#else
    mkdir(extractPath.c_str(), 0755);
#endif

    if (zipData.size() < 22)
    {
        cerr << "File too small to be a valid ZIP" << endl;
        return false;
    }

    // Check for ZIP file signature (PK)
    if (zipData[0] != 'P' || zipData[1] != 'K')
    {
        cerr << "Not a valid ZIP file" << endl;
        return false;
    }

    // Parse ZIP file using zlib
    if (!parseAndExtractZip(zipData, extractPath))
    {
        cerr << "Failed to extract ZIP file using zlib" << endl;
        return false;
    }

    cout << "ZIP extraction completed successfully using zlib" << endl;
    return true;
}

/**
 * @brief Extracts a wheel file to the specified virtual environment.
 * This function renames the wheel file to a zip file, extracts its contents
 * to a temporary directory, and then copies the extracted files
 * to the site-packages directory of the virtual environment.
 *
 * @param wheelPath The path to the wheel file to be extracted.
 * @param venvPath The path to the virtual environment where the package will be installed.
 *
 * @returns `True` if the extraction and installation were successful.
 */
bool DeadLock::extractWheelToVenv(string wheelPath, string venvPath)
{
    // Rename wheel to zip
    string sitePackagesPath;
    string zipPath;
    if (!renameWheelToZip(wheelPath, zipPath))
    {
        return false;
    }

    // Create temporary extraction directory
    string tempExtractPath = "temp_extract";
    if (!extractZipFile(zipPath, tempExtractPath))
    {
        return false;
    }

#ifdef _WIN32
    sitePackagesPath = venvPath + "\\Lib\\site-packages";
    _mkdir(tempExtractPath.c_str());
    _mkdir(sitePackagesPath.c_str());
    string copyCommand = "xcopy \"" + tempExtractPath + "\" \"" + sitePackagesPath + "\" /E /I /Y /Q";
    int result = system(copyCommand.c_str());
    if (result != 0)
    {
        cerr << "Failed to copy package contents to site-packages" << endl;
        return false;
    }
    system(("rmdir /S /Q \"" + tempExtractPath + "\"").c_str());
    _unlink(zipPath.c_str());
#else
    mkdir(tempExtractPath.c_str(), 0755);
    string libPath = venvPath + "/lib";
    DIR *dir = opendir(libPath.c_str());
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            string dirName = entry->d_name;
            if (dirName.find("python") == 0 && dirName != "." && dirName != "..")
            {
                sitePackagesPath = libPath + "/" + dirName + "/site-packages";
                break;
            }
        }
        closedir(dir);
    }

    if (sitePackagesPath.empty())
    {
        sitePackagesPath = venvPath + "/lib/python3/site-packages";
    }
    mkdir(sitePackagesPath.c_str(), 0755);
    string copyCommand = "cp -r \"" + tempExtractPath + "\"/* \"" + sitePackagesPath + "\"/";
    int result = system(copyCommand.c_str());
    if (result != 0)
    {
        cerr << "Failed to copy package contents to site-packages" << endl;
        return false;
    }
    system(("rm -rf \"" + tempExtractPath + "\"").c_str());
    unlink(zipPath.c_str());
#endif
    cout << "Package successfully extracted to virtual environment!" << endl;
    return true;
}

/**
 * @brief Parses a ZIP file and extracts its contents to the specified directory.
 *
 * This function reads the ZIP file data, locates the End of Central Directory (EOCD),
 * parses the central directory entries, and extracts each file to the specified path.
 *
 * @param zipData The raw data of the ZIP file as a vector of unsigned chars.
 * @param extractPath The directory where the contents of the ZIP file will be extracted.
 *
 * @returns `True` if the extraction was successful.
 */
bool DeadLock::parseAndExtractZip(vector<unsigned char> zipData, string extractPath)
{
    cout << "Parsing ZIP file with zlib decompression..." << endl;

    // Find End of Central Directory Record (EOCD)
    size_t zipSize = zipData.size();
    size_t eocdOffset = zipSize - 22; // Start from minimum EOCD size

    // Search for EOCD signature backwards from end of file
    bool foundEOCD = false;
    for (size_t i = eocdOffset; i > 0; i--)
    {
        if (i + 22 <= zipSize && readUint32(zipData.data(), i) == 0x06054b50)
        {
            eocdOffset = i;
            foundEOCD = true;
            break;
        }
    }

    if (!foundEOCD)
    {
        cerr << "Could not find End of Central Directory record" << endl;
        return false;
    }

    // Parse EOCD
    uint16_t numEntries = readUint16(zipData.data(), eocdOffset + 10);
    uint32_t centralDirSize = readUint32(zipData.data(), eocdOffset + 12);
    uint32_t centralDirOffset = readUint32(zipData.data(), eocdOffset + 16);

    cout << "Found " << numEntries << " entries in ZIP file" << endl;

    if (centralDirOffset + centralDirSize > zipSize)
    {
        cerr << "Invalid central directory offset or size" << endl;
        return false;
    }

    // Process each entry in the central directory
    size_t currentOffset = centralDirOffset;
    for (uint16_t i = 0; i < numEntries; i++)
    {
        if (currentOffset + 46 > zipSize)
        {
            cerr << "Invalid central directory entry offset" << endl;
            return false;
        }

        // Check central directory header signature
        if (readUint32(zipData.data(), currentOffset) != 0x02014b50)
        {
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
        if (currentOffset + 46 + filenameLength > zipSize)
        {
            cerr << "Invalid filename length" << endl;
            return false;
        }

        string filename(reinterpret_cast<char *>(zipData.data() + currentOffset + 46), filenameLength);
        // Skip directories
        if (filename.back() == '/' || filename.back() == '\\')
        {
            continue;
        }

        // Find local header
        if (localHeaderOffset + 30 > zipSize)
        {
            cerr << "Invalid local header offset" << endl;
            return false;
        }

        // Check local header signature
        if (readUint32(zipData.data(), localHeaderOffset) != 0x04034b50)
        {
            cerr << "Invalid local header signature" << endl;
            return false;
        }

        // Parse local header to get filename and extra field lengths
        uint16_t localFilenameLength = readUint16(zipData.data(), localHeaderOffset + 26);
        uint16_t localExtraFieldLength = readUint16(zipData.data(), localHeaderOffset + 28);

        // Calculate data offset
        size_t dataOffset = localHeaderOffset + 30 + localFilenameLength + localExtraFieldLength;

        if (dataOffset + compressedSize > zipSize)
        {
            cerr << "Invalid data offset or compressed size" << endl;
            return false;
        }

        // Create directory structure for the file
        string fullPath = extractPath + "/" + filename;
        if (!createDirectoryRecursive(fullPath))
        {
            cerr << "Failed to create directory for: " << fullPath << endl;
            return false;
        }
        // Extract and decompress file data
        vector<unsigned char> decompressedData(max(uncompressedSize, 1u)); // Ensure at least 1 byte for empty files

        if (compressionMethod == 0)
        {
            // No compression (stored)
            if (compressedSize != uncompressedSize)
            {
                cerr << "Size mismatch for stored file: " << filename << endl;
                return false;
            }
            if (compressedSize > 0)
            {
                memcpy(decompressedData.data(), zipData.data() + dataOffset, compressedSize);
            }
        }
        else if (compressionMethod == 8)
        {
            // Deflate compression
            // Handle empty files
            if (uncompressedSize != 0)
            {
                z_stream strm = {};
                strm.next_in = const_cast<unsigned char *>(zipData.data() + dataOffset);
                strm.avail_in = compressedSize;
                strm.next_out = decompressedData.data();
                strm.avail_out = uncompressedSize;

                // Initialize for raw deflate (no zlib header/trailer)
                int ret = inflateInit2(&strm, -MAX_WBITS);
                if (ret != Z_OK)
                {
                    cerr << "Failed to initialize zlib inflation for file: " << filename << endl;
                    return false;
                }

                ret = inflate(&strm, Z_FINISH);
                inflateEnd(&strm);

                if (ret != Z_STREAM_END || strm.total_out != uncompressedSize)
                {
                    cerr << "Failed to decompress file: " << filename << " (error: " << ret << ")" << endl;
                    return false;
                }
            }
        }
        else
        {
            cerr << "Unsupported compression method: " << compressionMethod << " for file: " << filename << endl;
            return false;
        }

        // Write decompressed data to file
        ofstream outFile(fullPath, ios::binary);
        if (!outFile)
        {
            cerr << "Failed to create output file: " << fullPath << endl;
            return false;
        }

        outFile.write(reinterpret_cast<char *>(decompressedData.data()), uncompressedSize);
        outFile.close();

        if (!outFile.good())
        {
            cerr << "Failed to write file: " << fullPath << endl;
            return false;
        }

        // Move to next central directory entry
        currentOffset += 46 + filenameLength + extraFieldLength + commentLength;
    }
    cout << "All entries processed successfully" << endl;
    return true;
}

bool DeadLock::generateDeadLockFile(string projectPath)
{
    string lockFilePath = getDeadLockFilePath();

    // Create the dead.lock file with initial structure
    json lockData = {
        {"generated", getCurrentTimestamp()},
        {"packages", json::array()},
        {"metadata", {{"python_version", ""}, {"platform",
#ifdef _WIN32
                                               "windows"
#elif __APPLE__
                                               "macos"
#else
                                               "linux"
#endif
                                              },
                      {"deadlock_version", "1.0.0"}}}};
    return writeDeadLockFile(lockData.dump(4), lockFilePath);
}

bool DeadLock::updateDeadLockFile(Package pkg)
{
    string deadData = readDeadLockFile(getDeadLockFilePath());
    json deadJSON;
    if (deadData.empty())
    {
        cerr << "Error reading dead.lock file" << endl;
        return false;
    }
    try
    {
        deadJSON = json::parse(deadData);
    }
    catch (json::exception &e)
    {
        cerr << "Error reading JSON in dead.lock:" << e.what() << "\n"
                  << "Regenerate? y/n: ";
        string opt;
        cin >> opt;
        if (opt == "y")
        {
            if (!generateDeadLockFile())
            {
                cerr << "Error generating dead.lock file" << endl;
                return false;
            }
            deadJSON = json::parse(readDeadLockFile(getDeadLockFilePath()));
        }
        else
        {
            return false;
        }
    }
    if (!deadJSON.contains("packages"))
    {
        deadJSON["packages"] = json::array();
    }
    deadJSON["packages"].push_back({{"name", pkg.name},
                                    {"version", pkg.version},
                                    {"source", pkg.source},
                                    {"install_date", pkg.installDate},
                                    {"dependencies", pkg.dependencies}});
    return writeDeadLockFile(deadJSON.dump(4), getDeadLockFilePath());
}

bool DeadLock::removeFromDeadLockFile(string packageName)
{
    // Find the package in loadedPackages vector
    auto it = find_if(loadedPackages.begin(), loadedPackages.end(),
                      [&packageName](Package &pkg)
                      {
                          return pkg.name == packageName;
                      });

    if (it == loadedPackages.end())
    {
        cerr << "Package " << packageName << " not found in dead.lock file" << endl;
        return false;
    }

    loadedPackages.erase(it);

    // Write updated dead.lock file
    string lockFilePath = getDeadLockFilePath();
    string content = generateDeadLockJson();

    bool success = writeDeadLockFile(content, lockFilePath);

    if (success)
    {
        cout << "Removed package " << packageName << " from dead.lock file" << endl;
    }

    return success;
}

bool DeadLock::isPackageInstalled(string packageName)
{
    string deadlockFileContent = readDeadLockFile(getDeadLockFilePath());
    if (deadlockFileContent.empty())
    {
        cerr << "Error reading dead.lock file" << endl;
        return false;
    }
    try
    {
        json jsonData = json::parse(deadlockFileContent);
        if (jsonData.contains("packages"))
        {
            for (const auto pkg : jsonData["packages"])
            {
                if (packageName == pkg)
                {
                    return true;
                }
            }
        }
        else
        {
            cout << "No packages installed in dead.lock file." << endl;
            return false;
        }
    }
    catch (json::exception &e)
    {
        cerr << "Error reading json object: " << e.what() << endl;
        return false;
    }
    catch (exception &e)
    {
        cerr << "Error finding " << packageName << "in dead.lock file: " << e.what() << endl;
        return false;
    }
    return false;
}

bool DeadLock::syncFromDeadLock(string projectPath)
{
    ifstream fileExists(getDeadLockFilePath());
    if (!fileExists)
    {
        cerr << "No dead.lock file found." << endl;
        return false;
    }
    cout << "Syncing packages from dead.lock file..." << endl;

    // Extract package names from loadedPackages for installation
    vector<string> packageNames;
    for (auto &pkg : loadedPackages)
    {
        packageNames.push_back(pkg.name);
    }

    if (packageNames.empty())
    {
        cout << "No packages found in dead.lock file to sync." << endl;
        return true;
    }

    // Create virtual environment if it doesn't exist
    if (!createVirtualEnvironment(".venv"))
    {
        cerr << "Error creating virtual environment" << endl;
    }

    // Install all packages using the existing logic
    return installPackages(packageNames);
}

/**
 * @brief Returns path of dead.lock file.
 *
 * @returns std:string The path of dead.lock file
 */
string DeadLock::getDeadLockFilePath()
{
    return "./dead.lock";
}

string DeadLock::getCurrentTimestamp()
{
    time_t now = time(0);
    char *timeStr = ctime(&now);
    string timestamp(timeStr);
    // Remove newline character
    if (!timestamp.empty() && timestamp.back() == '\n')
    {
        timestamp.pop_back();
    }
    return timestamp;
}

/**
 * @brief Gets ***necessary*** packages of the provided package.
 *
 * This function queries PYPI and retrieves info about all the required dependencies of the provided  package.
 *
 * @param packageName The name of the package whose dependencies are required.
 *
 * @returns An array containing names of required deoendencies.
 */
vector<string> DeadLock::getPackageDependencies(string packageName)
{
    // Get package info and extract dependencies
    string url = "https://pypi.org/pypi/" + packageName + "/json";
    string info = apiCaller(url);
    vector<string> dependencies;

    if (!info.empty())
    {
        try
        {
            json packageData = json::parse(info);
            if (packageData.contains("info") && packageData["info"].contains("requires_dist"))
            {
                for (auto &req : packageData["info"]["requires_dist"])
                {
                    if (!req.is_null())
                    {
                        string depString = req.get<string>();
                        // Check if dependency is optional
                        if (depString.find("extra") != string::npos)
                        {
                            continue;
                        }
                        // Extract package name (before any version specifiers)
                        size_t spacePos = depString.find(' ');
                        size_t parenPos = depString.find('(');
                        size_t semicolonPos = depString.find(';');
                        size_t gtPos = depString.find('>');
                        size_t ltPos = depString.find('<');
                        size_t eqPos = depString.find('=');
                        size_t endPos = (spacePos != string::npos ? spacePos : depString.length());
                        if (parenPos != string::npos && parenPos < endPos)
                            endPos = parenPos;
                        if (semicolonPos != string::npos && semicolonPos < endPos)
                            endPos = semicolonPos;
                        if (gtPos != string::npos && gtPos < endPos)
                            endPos = gtPos;
                        if (ltPos != string::npos && ltPos < endPos)
                            endPos = ltPos;
                        if (eqPos != string::npos && eqPos < endPos)
                            endPos = eqPos;
                        string depName = depString.substr(0, endPos);
                        if (!depName.empty() && find(dependencies.begin(), dependencies.end(), depName) == dependencies.end())
                        { // if package is not already in array, then push
                            dependencies.push_back(depName);
                        }
                    }
                }
            }
            else
            {
                cerr << "Error finding package version" << endl;
            }
        }
        catch (json::exception &e)
        {
            cerr << "Error parsing dependencies for " << packageName << ": " << e.what() << endl;
        }
    }
    return dependencies;
}

string DeadLock::generateDeadLockJson()
{
    json lockData = {
        {"version", "1.0"},
        {"generated", getCurrentTimestamp()},
        {"packages", json::array()},
        {{"python_version", ""},
         {"platform",
#ifdef _WIN32
          "windows"
#elif __APPLE__
          "macos"
#else
          "linux"
#endif
         },
         {"deadlock_version", "1.0.0"}}};

    // Add all packages from loadedPackages to the JSON
    for (auto &pkg : loadedPackages)
    {
        lockData["packages"][pkg.name] = {
            {"version", pkg.version},
            {"source", pkg.source},
            {"install_date", pkg.installDate}};
    }

    return lockData.dump(4);
}

bool DeadLock::writeDeadLockFile(string content, string filePath)
{
    ofstream file(filePath);
    if (!file)
    {
        cerr << "Failed to open dead.lock file for writing: " << filePath << endl;
        return false;
    }

    file << content;
    file.close();

    if (!file.good()) // File I/O operation completed successfully
    {
        cerr << "Failed to write to dead.lock file: " << filePath << endl;
        return false;
    }

    return true;
}

string DeadLock::readDeadLockFile(string filePath)
{
    ifstream file(filePath);
    if (!file)
    {
        return ""; // File doesn't exist
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

vector<Package> DeadLock::getInstalledPackages()
{
    // TODO: Write implementation
    return {};
}

void DeadLock::userOption()
{
    vector<string> packages;
    int type, option;
    cout << "What type of project do you want?\n1) Basic\n"
                 "2) Computer Vision\n"
                 "3) NLP\n"
                 "4) Empty"
              << endl;
    cin >> type;
    switch (type)
    {
    case 1:
        packages = {"pandas",
                    "numpy",
                    "scikit-learn",
                    "matplotlib",
                    "seaborn",
                    "scipy"};
        if (!installPackages(packages))
        {
            cout << "Error installing Packages" << endl;
            exit(-1);
        }
        break;
    case 2:
        cout << "With Which library you want to make this project?\n1) TensorFlow\t2) PyTorch\n"
                  << endl;
        cin >> option;
        switch (option)
        {
        case 1:
            packages = {
                "pandas",
                "numpy",
                "scikit-learn",
                "matplotlib",
                "seaborn",
                "tensorflow",
                "opencv-contrib-python",
                "openai",
                "keras"};
            break;
        case 2:
            packages = {
                "pandas",
                "numpy",
                "scikit-learn",
                "matplotlib",
                "seaborn",
                "torch",
                "torchvision",
                "opencv-contrib-python",
                "openai"};
            break;

        default:
            cout << "Invalid option chosen, ending operations" << endl;
            _rmdir(gProjectName.c_str());
            exit(-1);
            break;
        }
        if (!installPackages(packages))
        {
            cout << "Error installing Packages" << endl;
            exit(-1);
        }
        break;
    case 3:
        cout << "With Which library you want to make this project?\n1) TensorFlow\t2) PyTorch\n"
                  << endl;
        cin >> option;
        switch (option)
        {
        case 1:
            packages = {
                "pandas",
                "numpy",
                "scikit-learn",
                "matplotlib",
                "seaborn",
                "tensorflow",
                "nltk",
                "tokenizer",
                "embeddings",
                "openai",
                "keras"};
            break;
        case 2:
            packages = {
                "pandas",
                "numpy",
                "scikit-learn",
                "matplotlib",
                "seaborn",
                "torch",
                "nltk",
                "tokenizer",
                "embeddings",
                "openai"};
            break;

        default:
            cout << "Invalid option chosen, ending operations" << endl;
            _rmdir(gProjectName.c_str());
            exit(-1);
            break;
        }
        if (!installPackages(packages))
        {
            cout << "Error installing Packages" << endl;
            exit(-1);
        }
        break;
    case 4:
        break;
    default:
        cout << "Invalid option chosen, ending operations" << endl;
        _rmdir(gProjectName.c_str());
        exit(-1);
        break;
    }
}

bool DeadLock::isPkgInDeadLock(string packageName)
{
    string file = getDeadLockFilePath();
    string data = readDeadLockFile(file);
    // TODO: Write Implementation
    return true;
}

/**
 * @brief Uninstalls the specified packages from project and removes its reference in dead.lock
 *
 *
 * @param packages Array containing package(s) name to be removed
 *
 * @returns `True` if package is removed.
 */
bool DeadLock::uninstallPackages(vector<string> packages) {
    cout << "Uninstalling packages" << endl;
    // TODO: Write Implementation
    return true;
}
