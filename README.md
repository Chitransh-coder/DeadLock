# DeadLock

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![GitHub pull requests](https://img.shields.io/github/issues-pr/Chitransh-coder/DeadLock.svg)
![Last commit](https://img.shields.io/github/last-commit/Chitransh-coder/DeadLock.svg)
![Code size](https://img.shields.io/github/languages/code-size/Chitransh-coder/DeadLock.svg)
![Top language](https://img.shields.io/github/languages/top/Chitransh-coder/DeadLock.svg)

## Table of Contents

<details>
<summary>Click to expand</summary>

- [DeadLock](#deadlock)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Required Dependencies](#required-dependencies)
    - [Building from Source](#building-from-source)
    - [Creating a New Project](#creating-a-new-project)
    - [Installing Packages](#installing-packages)
    - [Getting Package Information](#getting-package-information)
    - [Syncing Dependencies](#syncing-dependencies)
    - [Listing Installed Packages](#listing-installed-packages)
  - [Commands](#commands)
  - [Project Templates](#project-templates)
    - [1. Basic Data Science](#1-basic-data-science)
    - [2. Computer Vision](#2-computer-vision)
    - [3. Natural Language Processing](#3-natural-language-processing)
    - [4. Empty Project](#4-empty-project)
  - [Dead.lock File](#deadlock-file)
    - [Structure](#structure)
  - [API Reference](#api-reference)
    - [Core Classes](#core-classes)
      - [`DeadLock`](#deadlock-1)
      - [`PackageDependency`](#packagedependency)
  - [Contributing](#contributing)
    - [Have an idea?](#have-an-idea)
    - [Want to Contribute with code?](#want-to-contribute-with-code)
      - [Contribution Guidelines](#contribution-guidelines)
    - [Code Style](#code-style)
    - [Project Structure](#project-structure)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)
  - [Status](#status)
</details>
<hr>
A smart way to manage your data science projects with pre-configured templates, dependency tracking and installation, etc.

## Features

- **Smart Project Initialization**: Create pre-configured data science projects with template selections.
- **Dependency Resolution**: Automatically resolve and install package dependencies.
- **Virtual Environment Management**: Built-in virtual environment creation and management.
- **PyPI Integration**: Direct integration with PyPI for package information and downloads.
- **Cross-Platform Support**: Works on Windows, macOS, and Linux.
- **Lock File Management**: `dead.lock` file for reproducible builds.
- **Template Projects**: Pre-configured templates for Basic, Computer Vision, and NLP projects.
- **Package Information**: Get detailed information about packages from PyPI.
- **Sync Functionality**: Sync packages from `dead.lock` file across environments.
- **Wheel File Handling**: Direct wheel file download and extraction.
- **Dependency Tracking**: Track and manage package dependencies with install dates.

## Installation

### Prerequisites

- **Python 3.7+** installed and available in PATH
- **C++ compiler** (GCC, Clang, or MSVC)
- **CMake 3.15+**
- **Git**

### Required Dependencies

- **libcurl** - For HTTP requests to PyPI
- **zlib** - For ZIP/wheel file extraction
- **nlohmann/json** - For JSON parsing
- **CLI11** - For command-line interface

### Building from Source

1. **Clone the repository**

   ```bash
   git clone https://github.com/Chitransh-coder/DeadLock.git
   cd DeadLock
   ```

2. **Install system dependencies**

   **Ubuntu/Debian:**

   ```bash
   sudo apt-get update
   sudo apt-get install build-essential cmake libcurl4-openssl-dev zlib1g-dev
   ```

   **macOS:**

   ```bash
   brew install cmake curl zlib
   ```

   **Windows:**

   ```bash
   # Install vcpkg first
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg install curl zlib
   ```

3. **Build the project**
   ```bash
   cmake --preset debug
   cmake --build build --config debug
   ```

### Creating a New Project

```bash
deadlock create <project-name>
```

This will:

- Set up a virtual environment.
- Generate project files (`.ipynb`, `.py`, `.gitignore`, `README.md`).
- Create a `dead.lock` file for dependency tracking.

### Installing Packages

```bash
# Install single package
deadlock install pandas

# Install multiple packages
deadlock install numpy pandas matplotlib seaborn
```

### Getting Package Information

```bash
deadlock info <package-name>
```

Example output:

```
Package: tensorflow
Latest version: 2.13.0
PyPI URL: https://pypi.org/project/tensorflow/
```

### Syncing Dependencies

```bash
deadlock sync
```

Installs all packages listed in the `dead.lock` file.

### Listing Installed Packages

```bash
deadlock list
```

Shows all packages from the `dead.lock` file with versions and dependencies.

## Commands

| Command                 | Description                 | Example                         |
| ----------------------- | --------------------------- | ------------------------------- |
| `create <name>`         | Create a new project        | `deadlock create my-project`    |
| `install <packages...>` | Install Python packages     | `deadlock install pandas numpy` |
| `info <package>`        | Get package information     | `deadlock info tensorflow`      |
| `sync`                  | Install from dead.lock file | `deadlock sync`                 |
| `list`                  | List installed packages     | `deadlock list`                 |

## Project Templates

DeadLock has pre-configured templates from which you can kickstart your project, you can choose between the following options:

### 1. Basic Data Science

Pre-installed packages for simple data analysis tasks:

- pandas
- numpy
- scikit-learn
- matplotlib
- seaborn
- scipy

### 2. Computer Vision

Choose between:

- **TensorFlow**: Installs TensorFlow, OpenCV, etc. in the virtual environment.
- **PyTorch**: Installs torch, torchvision, OpenCV, etc. in the isolated virtual environment.

### 3. Natural Language Processing

Choose between:

- **TensorFlow**: Installs TensorFlow in addition to tokenizer, nltk, embeddings, etc. in the virtual environment.
- **PyTorch**: Installs torch in addition to tokenizer, nltk, embeddings, etc. in the virtual environment.

### 4. Empty Project

No pre-installed packages, clean slate for custom setups.

## Dead.lock File

The `dead.lock` file ensures reproducible builds by tracking exact package versions and dependencies.

### Structure

```json
{
  "version": "1.0",
  "generated": "Mon Oct 23 14:30:45 2023",
  "packages": {
    "pandas": {
      "version": "2.1.0",
      "source": "pypi",
      "install_date": "Mon Oct 23 14:30:45 2023",
      "dependencies": ["numpy", "python-dateutil", "pytz"]
    }
  },
  "metadata": {
    "python_version": "3.9.7",
    "platform": "windows",
    "deadlock_version": "1.0.0"
  }
}
```

## API Reference

### Core Classes

#### `DeadLock`

Main class for dependency management.

**Key Methods:**

- `init(string projectName)` - Initialize new project
- `installPackages(vector<string> packages)` - Install multiple packages
- `getLatestVersion(string packageName)` - Get latest version from PyPI
- `getPackageDependencies(string packageName, string version)` - Get dependencies
- `syncFromDeadLock(string projectPath)` - Sync from lock file
- `loadDeadLockFile(string projectPath)` - Load existing lock file

#### `PackageDependency`

Structure representing a package dependency.

```cpp
struct PackageDependency {
    string name;
    string version;
    string source;
    string installDate;
    vector<string> dependencies;
};
```

## Contributing

Contributions are welcome. Please follow below guidelines if you have an idea or want a feature:

### Have an idea?

Open a [GitHub issue](https://github.com/Chitransh-coder/DeadLock/issues)

### Want to Contribute with code?

Open an issue and follow below steps:

#### Contribution Guidelines

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. **Make your changes**
4. **Test your Changes**
5. **Ensure code quality**
6. **Commit your changes**
   ```bash
   git commit -m "Add amazing feature"
   ```
7. **Push to your fork**
   ```bash
   git push origin feature/amazing-feature
   ```
8. **Create a Pull Request**

### Code Style

- Follow existing C++ coding conventions.
- Variable and function names should follow smallCamelCase naming scheme.
- Classes should follow UpperCamelCase naming scheme.
- Add comments for complex logic.
- Follow existing directory structure.

### Project Structure

```
DeadLock/
├── src/                    # Source files
│   ├── deadlock.cpp       # Main implementation
│   └── lock.cpp          # CLI interface
├── include/               # Header files
│   ├── deadlock.h        # Main header
│   ├── structs.h         # Data structures
├── third_party/          # External dependencies
└── CMakeLists.txt        # Build configuration
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [CLI11](https://github.com/CLIUtils/CLI11) for command-line parsing
- [nlohmann/json](https://github.com/nlohmann/json) for JSON handling
- [libcurl](https://curl.se/libcurl/) for HTTP requests
- [zlib](https://zlib.net/) for compression support

## Status

- ✅ Core functionality implemented
- ✅ Cross-platform support
- ✅ PyPI integration
- ✅ Virtual environment management
- 🔄 Dependency resolution improvements

---

Made with ❤️ for the data science community.
