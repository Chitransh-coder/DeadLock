# DeadLock Doxygen Documentation Build Guide

## Overview
This guide provides step-by-step instructions for generating professional API documentation for the DeadLock project using Doxygen.

## Prerequisites

### 1. Install Doxygen
Choose one of the following methods:

#### Windows (Recommended)
```powershell
# Using Chocolatey (if installed)
choco install doxygen

# Or using vcpkg (since your project uses vcpkg)
./vcpkg install doxygen:x64-windows
```

#### Windows (Direct Download)
- Download from: https://www.doxygen.nl/download.html  
- Choose the Windows installer and follow the installation wizard

#### macOS
```bash
brew install doxygen
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install doxygen
```

### 2. Optional Tools

#### Graphviz (for diagrams)
```powershell
# Windows with Chocolatey
choco install graphviz

# Or vcpkg
./vcpkg install graphviz:x64-windows
```

#### LaTeX (for PDF generation)
```powershell
# Windows with Chocolatey
choco install miktex

# Or use vcpkg
./vcpkg install texlive:x64-windows
```

## Step-by-Step Build Instructions

### Step 1: Generate Doxygen Configuration File

From the project root directory, generate a Doxyfile:

```powershell
# Create a default Doxyfile
doxygen -g Doxyfile
```

This creates a `Doxyfile` with default settings. You can edit this file to customize the documentation generation.

### Step 2: Configure the Doxyfile

Edit the `Doxyfile` and modify the following key settings:

```
# Project identification
PROJECT_NAME           = "DeadLock"
PROJECT_NUMBER         = "1.0.0"
PROJECT_BRIEF          = "A Project Manager made for Data Scientists"

# Source code location
INPUT                  = ./include ./src ./deadlock

# Output settings
OUTPUT_DIRECTORY       = ./docs
HTML_OUTPUT            = html
LATEX_OUTPUT           = latex

# Documentation extraction
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
RECURSIVE              = YES

# Include diagrams
HAVE_DOT               = YES
DOT_PATH               = "C:\Program Files\Graphviz\bin"  # Adjust path as needed
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
CLASS_DIAGRAMS         = YES

# Code highlighting
SOURCE_BROWSER         = YES
GENERATE_TREEVIEW      = YES

# Search functionality
SEARCHENGINE           = YES
SERVER_BASED_SEARCH    = NO

# Generate LaTeX/PDF
GENERATE_LATEX         = YES
LATEX_CMD_NAME         = pdflatex
```

### Step 3: Verify Comment Format

The DeadLock project already has correct Doxygen-style comments. Examples:

```cpp
/**
 * @brief Perform an HTTP Get request to the given URL Using cURL.
 *
 * This function initializes a cURL session sends the request, 
 * collect the response and return the server's reply as the string.
 *
 * @param url The target URL to which the HTTP GET request will be send.
 *
 * @returns string The response body from the server, or an empty string if the request fails.
 */
string DeadLock::apiCaller(string url)
```

**Format is CORRECT** ✓

### Step 4: Generate Documentation

From the project root, run:

```powershell
# Generate HTML documentation
doxygen Doxyfile
```

Or use a specific output format:

```powershell
# Generate only HTML
doxygen -g Doxyfile.html Doxyfile
doxygen Doxyfile.html

# Generate with custom Doxyfile location
doxygen path/to/Doxyfile
```

### Step 5: View the Generated Documentation

#### HTML Documentation
The generated HTML files will be in `./docs/html/`:

```powershell
# Open in default browser
Start-Process ./docs/html/index.html

# Or manually open:
# 1. Navigate to ./docs/html/
# 2. Open index.html in your web browser
```

#### PDF Documentation (if LaTeX is installed)
The generated LaTeX files will be in `./docs/latex/`:

```powershell
# Generate PDF from LaTeX
cd ./docs/latex
make pdf  # On Linux/macOS
# or
pdflatex refman.tex  # On Windows
```

## Advanced Configuration Options

### Custom Styling
Create a custom stylesheet in `doxygen/doxygen.css`:

```css
/* Customize colors, fonts, etc. */
body { font-family: Arial, sans-serif; }
```

Then in Doxyfile:
```
HTML_STYLESHEET        = doxygen/doxygen.css
```

### Exclude Files/Directories
```
EXCLUDE_PATTERNS       = */vcpkg/* */build/* */test/*
```

### Custom Header/Footer
```
HTML_HEADER            = doxygen/header.html
HTML_FOOTER            = doxygen/footer.html
```

### Generate Tags File
For cross-referencing with other projects:
```
GENERATE_TAGFILE       = ./docs/deadlock.tag
```

## Integration with CI/CD

### GitHub Actions Example
Create `.github/workflows/docs.yml`:

```yaml
name: Generate Documentation

on:
  push:
    branches: [ main, develop ]

jobs:
  docs:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Doxygen
        run: sudo apt-get install doxygen graphviz
      - name: Generate Docs
        run: doxygen Doxyfile
      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./docs/html
```

## Troubleshooting

### Issue: Doxygen command not found
**Solution:** Add Doxygen to PATH or use full path:
```powershell
# On Windows
"C:\Program Files\doxygen\bin\doxygen.exe" Doxyfile
```

### Issue: Graphviz not found
**Solution:** Set DOT_PATH in Doxyfile:
```
DOT_PATH               = "C:\Program Files\Graphviz\bin"
```

### Issue: No output generated
**Possible causes:**
- INPUT directory doesn't exist
- SOURCE_ROOT not set correctly
- File permissions issue

**Solution:**
1. Verify file paths in Doxyfile
2. Check `Doxyfile` exists in project root
3. Run with verbose output: `doxygen -d version Doxyfile`

### Issue: Comments not appearing in docs
**Solution:** Ensure EXTRACT_ALL is set to YES in Doxyfile:
```
EXTRACT_ALL            = YES
```

## Best Practices

1. **Keep comments updated** - Update documentation comments when code changes
2. **Use consistent formatting** - Follow the JavaDoc style consistently
3. **Document all public APIs** - Include @brief and @param for all functions
4. **Add examples** - Use @example tag for complex functions
5. **Generate regularly** - Make documentation generation part of your build process
6. **Version your docs** - Keep separate documentation for each release

## Comment Format Guide for This Project

The project uses JavaDoc-style Doxygen comments:

```cpp
/**
 * @brief Short description
 *
 * Longer description with more details.
 * Can span multiple lines.
 *
 * @param paramName Description of parameter
 * @param anotherParam Another parameter description
 *
 * @returns Description of return value
 *
 * @throws ExceptionType Description if applicable
 *
 * @note Important notes if any
 * @warning Warnings if any
 * @see RelatedFunction
 */
```

## Verification

✅ **Current Comment Format:** The comments in `src/deadlock.cpp` are properly formatted for Doxygen
✅ **Documentation Capability:** Full API documentation can be generated
✅ **All Methods Documented:** Functions have proper `@brief`, `@param`, and `@returns` tags

## Quick Start

To quickly generate documentation:

```powershell
# 1. Install Doxygen
choco install doxygen

# 2. Generate default Doxyfile
doxygen -g

# 3. Run Doxygen
doxygen Doxyfile

# 4. View HTML documentation
Start-Process ./docs/html/index.html
```

---

**Last Updated:** March 6, 2026
**Project:** DeadLock v1.0.0
