cmake_minimum_required(VERSION 3.25)
project(DudeDumper)

set(CMAKE_CXX_STANDARD 23)

add_executable(DudeDumper main.cpp structs.h memory.cpp memory.h memoryTest.cpp)
