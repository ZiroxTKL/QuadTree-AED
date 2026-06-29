#pragma once
#include "quadtree/QuadNode.hpp"
#include "core/Config.hpp"

#include <vector>
#include <memory>
#include <cstdint>

namespace qt {

    // Fachada del QuadTree. Construye el árbol a partir de un vector de
    // partículas y ofrece las consultas pidiendo siempre el conteo de
    // comparaciones para poder comparar contra fuerza bruta.
    class QuadTree {
    public:
        QuadTree(const Rectangle& bounds, int capacity, int maxDepth = 16);

        // Reconstruye el árbol completo desde cero con las partículas dadas.
        // Devuelve el número de comparaciones hechas durante la construcción.
        int64_t build(const std::vector<Particle>& particles);

        // Vacía el árbol.
        void clear();

        // Consulta por región rectangular. Acumula comparaciones en el contador.
        std::vector<const Particle*> queryRange(const Rectangle& range,
                                                int64_t& comparisons) const;

        // Consulta por radio de vecindad alrededor de un punto.
        std::vector<const Particle*> queryNeighbors(double x, double y, double radius,
                                                    int64_t& comparisons) const;

        // Límites de todos los nodos (para que el renderer dibuje las subdivisiones).
        std::vector<Rectangle> allBounds() const;

        int nodeCount() const;
        const Rectangle& bounds() const { return bounds_; }

    private:
        Rectangle bounds_;
        int capacity_;
        int maxDepth_;
        std::unique_ptr<QuadNode> root_;
    };

} // namespace qt
