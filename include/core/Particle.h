#pragma once

// Estructura de partícula sugerida en el enunciado del proyecto.
// Cada partícula tiene posición, velocidad y radio.
namespace qt {

    struct Particle {
        int id = 0;
        double x = 0.0,  y = 0.0;   // posición
        double vx = 0.0, vy = 0.0;  // velocidad
        double radius = 1.0;        // radio

        Particle() = default;
        Particle(int id_, double x_, double y_, double vx_, double vy_, double r_)
            : id(id_), x(x_), y(y_), vx(vx_), vy(vy_), radius(r_) {}
    };

    // Distancia euclidiana al cuadrado entre dos partículas.
    // Se usa la versión al cuadrado para evitar sqrt en bucles calientes.
    inline double distanceSq(const Particle& a, const Particle& b) {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    // ¿Se solapan los círculos de dos partículas? (posible colisión)
    inline bool overlap(const Particle& a, const Particle& b) {
        double r = a.radius + b.radius;
        return distanceSq(a, b) <= r * r;
    }

} // namespace qt
