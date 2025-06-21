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

# Find local ZLIB library
find_library(ZLIB_LIBRARY 
    NAMES zd
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib
    NO_DEFAULT_PATH
)

if(ZLIB_LIBRARY)
    message(STATUS "Found local ZLIB: ${ZLIB_LIBRARY}")
else()
    message(WARNING "ZLIB library not found")
endif()

# Fetch external dependencies
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(CLI11)

# Use local json.hpp instead of fetching
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
    add_definitions(-DCURL_STATICLIB)
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
    ${THIRD_PARTY_DIR}/curl
    ${THIRD_PARTY_DIR}/zlib
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
# target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Handle ZLIB linking
if(ZLIB_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARY})
endif()

# Add Windows networking libraries for CURL functionality
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wininet ws2_32 wldap32 crypt32 normaliz)
endif()

# Link filesystem library for C++17
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()
