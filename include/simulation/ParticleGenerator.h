#pragma once
#include "core/Particle.hpp"
#include "core/Config.hpp"
#include "utils/Random.hpp"

#include <vector>

namespace qt {

    // Genera datos sintéticos configurables. Implementa los tres escenarios
    // de distribución exigidos: uniforme, con clusters y con zona densa.
    class ParticleGenerator {
    public:
        explicit ParticleGenerator(const Config& cfg) : cfg_(cfg), rng_(cfg.seed) {}

        // Genera 'cfg.n' partículas según 'cfg.distribution'.
        std::vector<Particle> generate();

    private:
        std::vector<Particle> generateUniform();
        std::vector<Particle> generateClusters();
        std::vector<Particle> generateDense();

        // Asigna velocidad aleatoria (dirección uniforme, magnitud en [speedMin, speedMax]).
        void assignVelocity(Particle& p);
        double randomRadius();
        // Mantiene la posición dentro del espacio [0,width] x [0,height].
        void clampToSpace(Particle& p);

        const Config& cfg_;
        Random rng_;
    };

} // namespace qt
