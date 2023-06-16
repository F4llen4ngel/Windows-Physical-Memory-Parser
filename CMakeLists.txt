cmake_minimum_required(VERSION 3.25)
project(DudeDumper)

set(CMAKE_CXX_STANDARD 23)

add_executable(DudeDumper main.cpp memory.cpp)
add_executable(DudeDumperTest memoryTest.cpp memory.cpp)

find_package(doctest CONFIG REQUIRED)
target_link_libraries(DudeDumperTest PRIVATE doctest::doctest)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(DudeDumper PRIVATE imgui::imgui)
