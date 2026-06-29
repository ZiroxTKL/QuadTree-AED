#pragma once
#include "core/Rectangle.hpp"
#include "core/Circle.hpp"
#include "core/Particle.hpp"

#include <vector>
#include <memory>
#include <cstdint>

namespace qt {

    // Un nodo del QuadTree. Cubre una región rectangular del espacio.
    // Mientras tiene <= capacity partículas es una hoja; al superarla se
    // subdivide en cuatro cuadrantes (NW, NE, SW, SE).
    class QuadNode {
    public:
        QuadNode(const Rectangle& bounds, int capacity, int maxDepth, int depth = 0);

        // Inserta una partícula. Devuelve true si quedó dentro de este subárbol.
        // 'comparisons' acumula el número de comprobaciones realizadas.
        bool insert(const Particle* p, int64_t& comparisons);

        // Consulta todas las partículas dentro de una región rectangular.
        void queryRange(const Rectangle& range,
                        std::vector<const Particle*>& out,
                        int64_t& comparisons) const;

        // Consulta todas las partículas dentro de un círculo (radio de vecindad).
        void queryCircle(const Circle& circle,
                         std::vector<const Particle*>& out,
                         int64_t& comparisons) const;

        bool isLeaf() const { return !nw_; }
        const Rectangle& bounds() const { return bounds_; }
        int depth() const { return depth_; }

        // Recorre el árbol y junta los límites de cada nodo (para dibujar).
        void collectBounds(std::vector<Rectangle>& out) const;

        // Número total de nodos en este subárbol.
        int countNodes() const;

    private:
        void subdivide(int64_t& comparisons);

        Rectangle bounds_;
        int capacity_;
        int maxDepth_;
        int depth_;

        std::vector<const Particle*> particles_;  // partículas en esta hoja

        std::unique_ptr<QuadNode> nw_, ne_, sw_, se_;  // hijos (nulos si es hoja)
    };

} // namespace qt
