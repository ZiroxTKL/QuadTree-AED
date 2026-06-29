#include "renderer/DrawParticles.hpp"
#include <SDL2/SDL.h>
#include <unordered_set>

namespace qt {

void DrawParticles::fillCircle(SDL_Renderer* ren, int cx, int cy, int radius) {
    // Algoritmo de relleno por scanlines.
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx = static_cast<int>(SDL_sqrt(static_cast<double>(radius * radius - dy * dy)));
        SDL_RenderDrawLine(ren, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void DrawParticles::drawCircle(SDL_Renderer* ren, int cx, int cy, int radius) {
    // Algoritmo del punto medio (Bresenham para círculos).
    int x = radius, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(ren, cx + x, cy + y);
        SDL_RenderDrawPoint(ren, cx + y, cy + x);
        SDL_RenderDrawPoint(ren, cx - y, cy + x);
        SDL_RenderDrawPoint(ren, cx - x, cy + y);
        SDL_RenderDrawPoint(ren, cx - x, cy - y);
        SDL_RenderDrawPoint(ren, cx - y, cy - x);
        SDL_RenderDrawPoint(ren, cx + y, cy - x);
        SDL_RenderDrawPoint(ren, cx + x, cy - y);
        if (err <= 0) { y += 1; err += 2 * y + 1; }
        if (err > 0)  { x -= 1; err -= 2 * x + 1; }
    }
}

void DrawParticles::draw(SDL_Renderer* ren,
                         const Camera2D& cam,
                         const std::vector<Particle>& particles,
                         const std::vector<CollisionPair>& collisions) {
    // Conjunto de ids que están en alguna colisión.
    std::unordered_set<int> colliding;
    colliding.reserve(collisions.size() * 2);
    for (const auto& c : collisions) {
        colliding.insert(c.a);
        colliding.insert(c.b);
    }

    for (const Particle& p : particles) {
        int sx, sy;
        cam.worldToScreen(p.x, p.y, sx, sy);
        int r = cam.scaleLength(p.radius);

        if (colliding.count(p.id)) {
            // Colisión: rojo relleno.
            SDL_SetRenderDrawColor(ren, 226, 75, 74, 255);
            fillCircle(ren, sx, sy, r);
        } else {
            // Normal: azul tenue relleno + borde.
            SDL_SetRenderDrawColor(ren, 55, 138, 221, 200);
            fillCircle(ren, sx, sy, r);
            SDL_SetRenderDrawColor(ren, 12, 68, 124, 255);
            drawCircle(ren, sx, sy, r);
        }
    }
}

void DrawParticles::drawCandidates(SDL_Renderer* ren,
                                   const Camera2D& cam,
                                   const std::vector<Particle>& particles,
                                   const std::unordered_set<int>& candidateIds) {
    SDL_SetRenderDrawColor(ren, 239, 159, 39, 255);  // ámbar
    for (const Particle& p : particles) {
        if (!candidateIds.count(p.id)) continue;
        int sx, sy;
        cam.worldToScreen(p.x, p.y, sx, sy);
        int r = cam.scaleLength(p.radius) + 2;
        drawCircle(ren, sx, sy, r);
    }
}

} // namespace qt