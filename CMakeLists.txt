cmake_minimum_required(VERSION 4.0)
set(CMAKE_C_COMPILER C:/MinGW/bin/gcc.exe)
set(CMAKE_CXX_COMPILER C:/MinGW/bin/g++.exe)
project(DeadLock)

set(CMAKE_CXX_STANDARD 17)

add_executable(DeadLock deadlock.cpp lock.cpp)
