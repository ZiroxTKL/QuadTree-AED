#pragma once
#include "core/Particle.hpp"
#include "quadtree/QuadTree.hpp"
#include <vector>
#include <utility>
#include <cstdint>

namespace qt {

    // Un par de partículas que colisionan (índices en el vector de partículas).
    struct CollisionPair {
        int a;
        int b;
    };

    // Resultado de una pasada de detección de colisiones.
    struct CollisionResult {
        std::vector<CollisionPair> pairs;  // pares que se solapan
        int64_t comparisons = 0;           // comprobaciones de distancia realizadas
        int64_t candidates  = 0;           // candidatos devueltos por el QuadTree
    };

    // Detecta colisiones usando el QuadTree para encontrar vecinos cercanos.
    // Para cada partícula consulta su vecindad y comprueba solapamiento real.
    class Collision {
    public:
        // Detecta todos los pares que se solapan, evitando duplicados (a < b).
        static CollisionResult detect(const std::vector<Particle>& particles,
                                      const QuadTree& tree);
    };

} // namespace qt
