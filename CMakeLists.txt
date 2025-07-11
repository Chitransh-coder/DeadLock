cmake_minimum_required(VERSION 3.16)
project(DeadLock VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define source and include directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# Try to find packages using vcpkg first, then fall back to local libraries
find_package(CURL QUIET)
find_package(ZLIB QUIET)

# Check if we're using vcpkg
if(DEFINED ENV{VCPKG_ROOT} OR DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg for dependencies")
    
    # If vcpkg packages not found, show warning but continue
    if(NOT CURL_FOUND)
        message(WARNING "CURL not found via vcpkg, using local headers")
        set(CURL_INCLUDE_DIRS ${THIRD_PARTY_DIR}/curl)
        set(CURL_LIBRARIES "")
        set(CURL_FOUND TRUE)
    endif()
    
    if(NOT ZLIB_FOUND)
        message(WARNING "ZLIB not found via vcpkg, using local library")
        find_library(ZLIB_LIBRARY 
            NAMES zd
            PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib
            NO_DEFAULT_PATH
        )
        
        if(ZLIB_LIBRARY)
            message(STATUS "Found local ZLIB: ${ZLIB_LIBRARY}")
            set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
            set(ZLIB_INCLUDE_DIRS ${THIRD_PARTY_DIR}/zlib)
            set(ZLIB_FOUND TRUE)
        else()
            message(WARNING "ZLIB library not found")
        endif()
    endif()
else()
    message(STATUS "Not using vcpkg, using local libraries")
    # Use local libraries
    set(CURL_INCLUDE_DIRS ${THIRD_PARTY_DIR}/curl)
    set(CURL_LIBRARIES "")
    set(CURL_FOUND TRUE)
    
    find_library(ZLIB_LIBRARY 
        NAMES zd
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib
        NO_DEFAULT_PATH
    )
    
    if(ZLIB_LIBRARY)
        message(STATUS "Found local ZLIB: ${ZLIB_LIBRARY}")
        set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
        set(ZLIB_INCLUDE_DIRS ${THIRD_PARTY_DIR}/zlib)
        set(ZLIB_FOUND TRUE)
    else()
        message(WARNING "ZLIB library not found")
    endif()
endif()

# Fetch external dependencies
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(CLI11)

# Use local json.hpp instead of fetching, use this if CMake can't find original json.hpp
# FetchContent_Declare(
#     nlohmann_json
#     GIT_REPOSITORY https://github.com/nlohmann/json
#     GIT_TAG v3.12.0
# )
# FetchContent_MakeAvailable(nlohmann_json)

# Add Windows-specific configuration
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DNOMINMAX)
endif()

# Define source files
set(SOURCES
    ${SRC_DIR}/deadlock.cpp
    ${SRC_DIR}/lock.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)

# Link CURL
if(TARGET CURL::libcurl)
    target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
elseif(CURL_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})
endif()

# Link ZLIB
if(TARGET ZLIB::ZLIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
elseif(ZLIB_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})
endif()

# Add Windows networking libraries for CURL functionality
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wininet ws2_32 wldap32 crypt32 normaliz)
    target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB)
endif()

# Link filesystem library for C++17
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()
