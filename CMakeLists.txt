cmake_minimum_required(VERSION 3.26)
project(DeadLock VERSION 1.0.0)

# Define source and include directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES
    ${SRC_DIR}/deadlock.cpp
    ${SRC_DIR}/lock.cpp
)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg.exe")
        message(STATUS "Bootstrapping vcpkg...")
        execute_process(
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg"
        )
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)

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

pybind11_add_module(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    ZLIB::ZLIB
    nlohmann_json::nlohmann_json)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${Python_INCLUDE_DIRS}
)