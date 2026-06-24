cmake_minimum_required(VERSION 3.16)
project(quadtree_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qt_core
    core/QuadTree.cpp
    core/Simulation.cpp
    core/BruteForce.cpp
    core/Distribution.cpp
)
target_include_directories(qt_core PUBLIC core)

add_executable(bench bench/benchmark.cpp)
target_link_libraries(bench PRIVATE qt_core)