#include "simulation/ParticleGenerator.hpp"
#include <cmath>

namespace qt {

std::vector<Particle> ParticleGenerator::generate() {
    switch (cfg_.distribution) {
        case Distribution::Clusters: return generateClusters();
        case Distribution::Dense:    return generateDense();
        default:                     return generateUniform();
    }
}

double ParticleGenerator::randomRadius() {
    if (cfg_.radiusMax <= cfg_.radiusMin) return cfg_.radiusMin;
    return rng_.uniform(cfg_.radiusMin, cfg_.radiusMax);
}

void ParticleGenerator::assignVelocity(Particle& p) {
    double angle = rng_.uniform(0.0, 2.0 * M_PI);
    double speed = rng_.uniform(cfg_.speedMin, cfg_.speedMax);
    p.vx = std::cos(angle) * speed;
    p.vy = std::sin(angle) * speed;
}

void ParticleGenerator::clampToSpace(Particle& p) {
    if (p.x < p.radius)               p.x = p.radius;
    if (p.x > cfg_.width  - p.radius) p.x = cfg_.width  - p.radius;
    if (p.y < p.radius)               p.y = p.radius;
    if (p.y > cfg_.height - p.radius) p.y = cfg_.height - p.radius;
}

// --- Distribución uniforme: posiciones repartidas por todo el espacio ---
std::vector<Particle> ParticleGenerator::generateUniform() {
    std::vector<Particle> ps;
    ps.reserve(cfg_.n);
    for (int i = 0; i < cfg_.n; ++i) {
        Particle p;
        p.id = i;
        p.radius = randomRadius();
        p.x = rng_.uniform(0.0, cfg_.width);
        p.y = rng_.uniform(0.0, cfg_.height);
        assignVelocity(p);
        clampToSpace(p);
        ps.push_back(p);
    }
    return ps;
}

// --- Distribución con clusters: varios grupos gaussianos ---
std::vector<Particle> ParticleGenerator::generateClusters() {
    std::vector<Particle> ps;
    ps.reserve(cfg_.n);

    int k = cfg_.numClusters > 0 ? cfg_.numClusters : 1;

    // Centros de cada cluster, repartidos con margen respecto a los bordes.
    std::vector<std::pair<double,double>> centers;
    double marginX = cfg_.width  * 0.15;
    double marginY = cfg_.height * 0.15;
    for (int c = 0; c < k; ++c) {
        centers.emplace_back(rng_.uniform(marginX, cfg_.width  - marginX),
                             rng_.uniform(marginY, cfg_.height - marginY));
    }

    for (int i = 0; i < cfg_.n; ++i) {
        int c = rng_.uniformInt(0, k - 1);
        Particle p;
        p.id = i;
        p.radius = randomRadius();
        p.x = rng_.normal(centers[c].first,  cfg_.clusterStd);
        p.y = rng_.normal(centers[c].second, cfg_.clusterStd);
        assignVelocity(p);
        clampToSpace(p);
        ps.push_back(p);
    }
    return ps;
}

// --- Distribución con zona de alta densidad: una fracción grande de las
//     partículas concentrada en un cuadrado pequeño, el resto disperso ---
std::vector<Particle> ParticleGenerator::generateDense() {
    std::vector<Particle> ps;
    ps.reserve(cfg_.n);

    double denseW = cfg_.width  * cfg_.denseSize;
    double denseH = cfg_.height * cfg_.denseSize;
    double denseX = (cfg_.width  - denseW) * 0.5;  // zona densa centrada
    double denseY = (cfg_.height - denseH) * 0.5;

    int nDense = static_cast<int>(cfg_.n * cfg_.denseFraction);

    for (int i = 0; i < cfg_.n; ++i) {
        Particle p;
        p.id = i;
        p.radius = randomRadius();
        if (i < nDense) {
            // dentro de la zona densa
            p.x = rng_.uniform(denseX, denseX + denseW);
            p.y = rng_.uniform(denseY, denseY + denseH);
        } else {
            // disperso por todo el espacio
            p.x = rng_.uniform(0.0, cfg_.width);
            p.y = rng_.uniform(0.0, cfg_.height);
        }
        assignVelocity(p);
        clampToSpace(p);
        ps.push_back(p);
    }
    return ps;
}

} // namespace qt
