#include "simulation/Simulation.hpp"
#include "simulation/ParticleGenerator.hpp"
#include "utils/Timer.hpp"

namespace qt {

Simulation::Simulation(const Config& cfg)
    : cfg_(cfg), physics_(cfg_) {
    // Generar el estado inicial de partículas.
    ParticleGenerator gen(cfg_);
    particles_ = gen.generate();

    // Crear el QuadTree cubriendo todo el espacio.
    Rectangle space(0.0, 0.0, cfg_.width, cfg_.height);
    tree_ = std::make_unique<QuadTree>(space, cfg_.capacity, cfg_.maxDepth);
    tree_->build(particles_);
}

FrameStats Simulation::stepFrame() {
    FrameStats stats;
    Timer total;

    // 1) Mover partículas.
    Timer t1;
    physics_.step(particles_);
    stats.moveMs = t1.elapsedMs();

    // 2) Reconstruir el QuadTree con las nuevas posiciones.
    Timer t2;
    stats.buildComparisons = tree_->build(particles_);
    stats.buildMs = t2.elapsedMs();

    // 3) Detectar colisiones usando el QuadTree.
    Timer t3;
    CollisionResult col = Collision::detect(particles_, *tree_);
    stats.collideMs = t3.elapsedMs();

    stats.queryComparisons = col.comparisons;
    stats.candidates       = col.candidates;
    stats.collisions       = static_cast<int>(col.pairs.size());
    stats.nodes            = tree_->nodeCount();
    lastCollisions_        = std::move(col.pairs);

    stats.totalMs = total.elapsedMs();
    ++frame_;
    return stats;
}

} // namespace qt
