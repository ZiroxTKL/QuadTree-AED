#pragma once
#include "core/Particle.hpp"
#include "core/Config.hpp"
#include <vector>

namespace qt {

    // Integra el movimiento de las partículas frame por frame y las hace
    // rebotar contra los bordes del espacio.
    class Physics {
    public:
        explicit Physics(const Config& cfg) : cfg_(cfg) {}

        // Avanza un paso de tiempo dt: actualiza posiciones y aplica rebotes.
        void step(std::vector<Particle>& particles) const;

    private:
        void integrate(Particle& p, double dt) const;
        void bounce(Particle& p) const;

        const Config& cfg_;
    };

} // namespace qt
