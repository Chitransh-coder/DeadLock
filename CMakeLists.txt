cmake_minimum_required(VERSION 4.0)
set(CMAKE_C_COMPILER C:/MinGW/bin/gcc.exe)
set(CMAKE_CXX_COMPILER C:/MinGW/bin/g++.exe)
project(DeadLock)

include(FetchContent)

FetchContent_Declare(
    CLI
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG 4160d259d961cd393fd8d67590a8c7d210207348
)
FetchContent_MakeAvailable(CLI)

set(CMAKE_CXX_STANDARD 17)

add_executable(DeadLock deadlock.cpp lock.cpp)

target_link_libraries(${PROJECT_NAME} CLI11::CLI11)
