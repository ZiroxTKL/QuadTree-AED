#pragma once
#include "quadtree/QuadTree.hpp"
#include "renderer/Camera2D.hpp"

struct SDL_Renderer;

namespace qt {

    // Dibuja las subdivisiones (límites de los nodos) del QuadTree como
    // rectángulos. Permite ver cómo se adapta el árbol a la densidad.
    class DrawQuadTree {
    public:
        static void draw(SDL_Renderer* ren,
                         const Camera2D& cam,
                         const QuadTree& tree);
    };

} // namespace qt