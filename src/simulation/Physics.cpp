#include "simulation/Physics.hpp"

namespace qt {

void Physics::integrate(Particle& p, double dt) const {
    p.x += p.vx * dt;
    p.y += p.vy * dt;
}

void Physics::bounce(Particle& p) const {
    // Rebote elástico contra las cuatro paredes del espacio.
    if (p.x - p.radius < 0.0) {
        p.x = p.radius;
        p.vx = -p.vx;
    } else if (p.x + p.radius > cfg_.width) {
        p.x = cfg_.width - p.radius;
        p.vx = -p.vx;
    }
    if (p.y - p.radius < 0.0) {
        p.y = p.radius;
        p.vy = -p.vy;
    } else if (p.y + p.radius > cfg_.height) {
        p.y = cfg_.height - p.radius;
        p.vy = -p.vy;
    }
}

void Physics::step(std::vector<Particle>& particles) const {
    for (Particle& p : particles) {
        integrate(p, cfg_.dt);
        bounce(p);
    }
}

} // namespace qt
