cmake_minimum_required(VERSION 3.16)
project(QuadTreeSimulator LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Opciones de configuracion
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de build" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
add_compile_options(-Wall -Wextra)

# WITH_RENDERER ON intenta compilar la visualizacion con SDL2.
# Si SDL2 no se encuentra, se desactiva automaticamente y se compila
# solo el modo benchmark/headless.
option(WITH_RENDERER "Compilar el visualizador SDL2" ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# ---------------------------------------------------------------------------
# Libreria core (todo menos el renderer y el main)
# ---------------------------------------------------------------------------
set(CORE_SOURCES
        src/core/Particle.cpp
        src/core/Rectangle.cpp
        src/core/Circle.cpp
        src/core/Config.cpp
        src/quadtree/QuadNode.cpp
        src/quadtree/QuadTree.cpp
        src/simulation/Physics.cpp
        src/simulation/ParticleGenerator.cpp
        src/simulation/Collision.cpp
        src/simulation/Simulation.cpp
        src/benchmark/BruteForce.cpp
        src/benchmark/Benchmark.cpp
        src/benchmark/Statistics.cpp
        src/utils/Random.cpp
        src/utils/Timer.cpp
        src/utils/Logger.cpp
)

add_library(qtcore STATIC ${CORE_SOURCES})

# ---------------------------------------------------------------------------
# Renderer opcional (SDL2)
# ---------------------------------------------------------------------------
set(RENDERER_AVAILABLE OFF)
if(WITH_RENDERER)
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        set(RENDERER_AVAILABLE ON)
        message(STATUS "SDL2 encontrado: se compila el visualizador.")
        add_library(qtrenderer STATIC
                src/renderer/Camera2D.cpp
                src/renderer/DrawParticles.cpp
                src/renderer/DrawQuadTree.cpp
                src/renderer/UI.cpp
                src/renderer/Renderer.cpp
        )
        target_include_directories(qtrenderer PUBLIC ${SDL2_INCLUDE_DIRS})
        target_link_libraries(qtrenderer PUBLIC qtcore ${SDL2_LIBRARIES})
        target_compile_definitions(qtrenderer PUBLIC WITH_RENDERER)
    else()
        message(WARNING "SDL2 no encontrado: se compila solo el modo benchmark/headless.")
    endif()
endif()

# ---------------------------------------------------------------------------
# Ejecutable principal
# ---------------------------------------------------------------------------
add_executable(simulator src/main.cpp)
if(RENDERER_AVAILABLE)
    target_link_libraries(simulator PRIVATE qtcore qtrenderer)
    target_compile_definitions(simulator PRIVATE WITH_RENDERER)
else()
    target_link_libraries(simulator PRIVATE qtcore)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
enable_testing()

add_executable(test_quadtree  tests/test_quadtree.cpp)
add_executable(test_collision tests/test_collision.cpp)
add_executable(test_generator tests/test_generator.cpp)

foreach(t test_quadtree test_collision test_generator)
    target_link_libraries(${t} PRIVATE qtcore)
    target_include_directories(${t} PRIVATE ${CMAKE_SOURCE_DIR}/tests)
    add_test(NAME ${t} COMMAND ${t})
endforeach()
