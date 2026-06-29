cmake_minimum_required(VERSION 4.0)
project(QuadTree)

set(CMAKE_CXX_STANDARD 20)

add_executable(QuadTree main.cpp
        src/main.cpp
        include/core/Particle.h
        include/core/Rectangle.h
        include/core/Circle.h
        include/core/Config.h
        include/quadtree/Quadtree.h
        include/quadtree/QuadNode.h
        include/simulation/Simulation.h
        include/simulation/Collision.h
        include/simulation/ParticleGenerator.h
        include/simulation/Physics.h
        include/benchmark/BruteForce.h
        include/benchmark/Benchmark.h
        include/benchmark/Statistics.h
        include/renderer/Renderer.h
        include/renderer/Camera2D.h
        include/renderer/DrawQuadTree.h
        include/renderer/DrawParticles.h
        include/renderer/UI.h
        include/utils/Random.h
        include/utils/Timer.h
        include/utils/Logger.h
        src/renderer/Renderer.cpp
        src/utils/Random.cpp
        src/renderer/Camera2D.cpp
        src/utils/Timer.cpp
        src/utils/Logger.cpp
        src/simulation/Simulation.cpp
        src/simulation/Collision.cpp
        src/simulation/ParticleGenerator.cpp
        src/simulation/Physics.cpp
        src/renderer/DrawQuadTree.cpp
        src/renderer/DrawParticles.cpp
        src/renderer/UI.cpp
        src/quadtree/QuadTree.cpp
        src/quadtree/QuadNode.cpp
        src/core/Particle.cpp
        src/core/Rectangle.cpp
        src/core/Circle.cpp
        src/core/Config.cpp
        src/benchmark/BruteForce.cpp
        src/benchmark/Benchmark.cpp
        src/benchmark/Statistics.cpp)
