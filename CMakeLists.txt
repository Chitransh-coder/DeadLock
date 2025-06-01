cmake_minimum_required(VERSION 3.10)
project(DeadLock, LANGUAGE CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(DeadLock deadlock.cpp lock.cpp)
