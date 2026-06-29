#include "renderer/DrawQuadTree.hpp"
#include <SDL2/SDL.h>

namespace qt {

void DrawQuadTree::draw(SDL_Renderer* ren,
                        const Camera2D& cam,
                        const QuadTree& tree) {
    // Color gris tenue para las líneas de subdivisión.
    SDL_SetRenderDrawColor(ren, 136, 135, 128, 120);

    for (const Rectangle& b : tree.allBounds()) {
        int x0, y0, x1, y1;
        cam.worldToScreen(b.x, b.y, x0, y0);
        cam.worldToScreen(b.right(), b.bottom(), x1, y1);

        SDL_Rect rect;
        rect.x = x0;
        rect.y = y0;
        rect.w = x1 - x0;
        rect.h = y1 - y0;
        SDL_RenderDrawRect(ren, &rect);
    }
}

} // namespace qt
