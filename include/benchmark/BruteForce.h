#pragma once
#include "core/Particle.hpp"
#include "simulation/Collision.hpp"
#include <vector>
#include <cstdint>

namespace qt {

    // Solución de fuerza bruta contra la cual se compara el QuadTree.
    // Revisa todos los pares posibles -> O(n^2).
    class BruteForce {
    public:
        // Detecta colisiones comparando cada par exactamente una vez.
        static CollisionResult detectCollisions(const std::vector<Particle>& particles);

        // Consulta de vecinos a un punto: recorre todas las partículas.
        static std::vector<const Particle*> queryNeighbors(
            const std::vector<Particle>& particles,
            double x, double y, double radius,
            int64_t& comparisons);
    };

} // namespace qt
