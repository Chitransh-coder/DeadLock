cmake_minimum_required(VERSION 3.16)
project(DeadLock VERSION 1.0)

# Define source and include directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/third_party)

# Try to find packages using vcpkg first, then fall back to local libraries
find_package(CURL QUIET)
find_package(ZLIB QUIET)

# Fetch external dependencies
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

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
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ZLIB::ZLIB
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Link filesystem library for C++17
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()
