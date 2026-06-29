#pragma once
#include "core/Particle.hpp"
#include "simulation/Collision.hpp"
#include "renderer/Camera2D.hpp"
#include <vector>
#include <unordered_set>

struct SDL_Renderer;

namespace qt {

    // Dibuja las partículas como círculos. Las que colisionan se resaltan,
    // y opcionalmente se dibujan los candidatos devueltos por una consulta.
    class DrawParticles {
    public:
        static void draw(SDL_Renderer* ren,
                         const Camera2D& cam,
                         const std::vector<Particle>& particles,
                         const std::vector<CollisionPair>& collisions);

        // Resalta un conjunto de candidatos (por id) con otro color.
        static void drawCandidates(SDL_Renderer* ren,
                                   const Camera2D& cam,
                                   const std::vector<Particle>& particles,
                                   const std::unordered_set<int>& candidateIds);

    private:
        static void fillCircle(SDL_Renderer* ren, int cx, int cy, int radius);
        static void drawCircle(SDL_Renderer* ren, int cx, int cy, int radius);
    };

} // namespace qt