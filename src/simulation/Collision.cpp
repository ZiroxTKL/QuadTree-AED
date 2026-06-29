#include "simulation/Collision.hpp"
#include <algorithm>

namespace qt {

CollisionResult Collision::detect(const std::vector<Particle>& particles,
                                  const QuadTree& tree) {
    CollisionResult result;

    // Radio máximo global: garantiza que el radio de búsqueda capture
    // cualquier vecino que pueda solaparse, incluso si es más grande que p.
    // Dos partículas colisionan si dist <= radius_p + radius_q, así que
    // buscar con radio (radius_p + maxRadius) nunca pierde un par válido.
    double maxRadius = 0.0;
    for (const Particle& p : particles) {
        maxRadius = std::max(maxRadius, p.radius);
    }

    for (const Particle& p : particles) {
        double searchR = p.radius + maxRadius;

        int64_t cmp = 0;
        std::vector<const Particle*> neighbors =
            tree.queryNeighbors(p.x, p.y, searchR, cmp);

        result.comparisons += cmp;
        result.candidates  += static_cast<int64_t>(neighbors.size());

        for (const Particle* q : neighbors) {
            // Evitar comparar consigo misma y contar cada par una sola vez.
            if (q->id <= p.id) continue;
            ++result.comparisons;
            if (overlap(p, *q)) {
                result.pairs.push_back({p.id, q->id});
            }
        }
    }

    return result;
}

} // namespace qt
