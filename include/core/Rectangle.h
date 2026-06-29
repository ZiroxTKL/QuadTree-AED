#pragma once
#include "core/Particle.hpp"

// Rectángulo alineado a los ejes (AABB). Define la región que cubre
// un nodo del QuadTree. Origen en la esquina superior-izquierda (x, y),
// con ancho w y alto h.
namespace qt {

    struct Rectangle {
        double x = 0.0, y = 0.0;  // esquina superior izquierda
        double w = 0.0, h = 0.0;  // ancho y alto

        Rectangle() = default;
        Rectangle(double x_, double y_, double w_, double h_)
            : x(x_), y(y_), w(w_), h(h_) {}

        double cx() const { return x + w * 0.5; }  // centro X
        double cy() const { return y + h * 0.5; }  // centro Y
        double right()  const { return x + w; }
        double bottom() const { return y + h; }

        // ¿El punto (px, py) está dentro del rectángulo?
        bool contains(double px, double py) const {
            return px >= x && px < x + w && py >= y && py < y + h;
        }

        bool contains(const Particle& p) const {
            return contains(p.x, p.y);
        }

        // ¿Este rectángulo intersecta con otro? (para podar ramas en queries)
        bool intersects(const Rectangle& o) const {
            return !(o.x > right()  ||
                     o.right() < x   ||
                     o.y > bottom()  ||
                     o.bottom() < y);
        }

        // ¿Intersecta con un círculo de centro (px, py) y radio r?
        // Se calcula el punto del rectángulo más cercano al centro del círculo.
        bool intersectsCircle(double px, double py, double r) const {
            double nearestX = px < x ? x : (px > right()  ? right()  : px);
            double nearestY = py < y ? y : (py > bottom() ? bottom() : py);
            double dx = px - nearestX;
            double dy = py - nearestY;
            return (dx * dx + dy * dy) <= r * r;
        }
    };

} // namespace qt
