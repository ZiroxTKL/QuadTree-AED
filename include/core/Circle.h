#pragma once
#include "core/Rectangle.hpp"

// Círculo de consulta: representa el "radio de vecindad" alrededor de
// un punto. Se usa en las queries de vecinos cercanos del QuadTree.
namespace qt {

    struct Circle {
        double cx = 0.0, cy = 0.0;  // centro
        double r = 0.0;             // radio

        Circle() = default;
        Circle(double cx_, double cy_, double r_) : cx(cx_), cy(cy_), r(r_) {}

        // ¿El punto (px, py) está dentro del círculo?
        bool contains(double px, double py) const {
            double dx = px - cx;
            double dy = py - cy;
            return (dx * dx + dy * dy) <= r * r;
        }

        // ¿Intersecta con un rectángulo? (delega en Rectangle)
        bool intersects(const Rectangle& rect) const {
            return rect.intersectsCircle(cx, cy, r);
        }

        // Caja envolvente del círculo: útil para una primera poda rápida.
        Rectangle boundingBox() const {
            return Rectangle(cx - r, cy - r, 2 * r, 2 * r);
        }
    };

} // namespace qt
