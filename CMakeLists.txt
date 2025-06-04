cmake_minimum_required(VERSION 3.10)
project(DeadLock)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define source and include directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Fetch external dependencies
include(FetchContent)
FetchContent_Declare(
    CLI
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG 4160d259d961cd393fd8d67590a8c7d210207348
)
FetchContent_MakeAvailable(CLI)

# Add Windows-specific configuration to prevent include errors
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WIN32_WINNT=0x0601)
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
    ${cli_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
