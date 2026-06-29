#pragma once
#include "core/Config.hpp"
#include "core/Particle.hpp"
#include "quadtree/QuadTree.hpp"
#include "simulation/Physics.hpp"
#include "simulation/Collision.hpp"

#include <vector>
#include <cstdint>

namespace qt {

    // Métricas de un solo frame.
    struct FrameStats {
        double moveMs    = 0.0;   // tiempo en mover partículas
        double buildMs   = 0.0;   // tiempo en reconstruir el QuadTree
        double collideMs = 0.0;   // tiempo en detectar colisiones
        double totalMs   = 0.0;   // tiempo total del frame
        int64_t buildComparisons = 0;
        int64_t queryComparisons = 0;
        int64_t candidates       = 0;
        int     collisions       = 0;
        int     nodes            = 0;
    };

    // Orquesta la simulación: genera partículas y ejecuta el loop
    // mover -> reconstruir QuadTree -> detectar colisiones.
    class Simulation {
    public:
        explicit Simulation(const Config& cfg);

        // Avanza un frame y devuelve sus métricas.
        FrameStats stepFrame();

        const std::vector<Particle>& particles() const { return particles_; }
        const QuadTree& tree() const { return *tree_; }
        const Config& config() const { return cfg_; }

        // Última lista de colisiones detectadas (para el renderer).
        const std::vector<CollisionPair>& lastCollisions() const { return lastCollisions_; }

        int frameIndex() const { return frame_; }

    private:
        Config cfg_;
        std::vector<Particle> particles_;
        std::unique_ptr<QuadTree> tree_;
        Physics physics_;
        std::vector<CollisionPair> lastCollisions_;
        int frame_ = 0;
    };

} // namespace qt
