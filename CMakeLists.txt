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

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.0
    GIT_SHALLOW    TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_library(qt_viz viz/Renderer.cpp)
target_include_directories(qt_viz PUBLIC viz core)
target_link_libraries(qt_viz PUBLIC qt_core raylib)

add_executable(viz_app app/main.cpp)
target_link_libraries(viz_app PRIVATE qt_viz)

if (WIN32)
    if (MSVC)
        set_target_properties(viz_app PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    else()
        target_link_options(viz_app PRIVATE -mwindows)
    endif()
endif()