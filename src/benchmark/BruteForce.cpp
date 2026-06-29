#include "benchmark/BruteForce.hpp"

namespace qt {

CollisionResult BruteForce::detectCollisions(const std::vector<Particle>& particles) {
    CollisionResult result;
    const size_t n = particles.size();

    // Cada par (i, j) con i < j se revisa una sola vez.
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            ++result.comparisons;
            if (overlap(particles[i], particles[j])) {
                result.pairs.push_back({particles[i].id, particles[j].id});
            }
        }
    }
    // En fuerza bruta el número de "candidatos" equivale a las comparaciones.
    result.candidates = result.comparisons;
    return result;
}

std::vector<const Particle*> BruteForce::queryNeighbors(
        const std::vector<Particle>& particles,
        double x, double y, double radius,
        int64_t& comparisons) {
    std::vector<const Particle*> out;
    double r2 = radius * radius;
    for (const Particle& p : particles) {
        ++comparisons;
        double dx = p.x - x;
        double dy = p.y - y;
        if (dx * dx + dy * dy <= r2) {
            out.push_back(&p);
        }
    }
    return out;
}

} // namespace qt
