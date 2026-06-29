#include "renderer/UI.hpp"
#include <SDL2/SDL.h>
#include <cstdio>

namespace qt {

    void UI::drawQueryCircle(SDL_Renderer* ren,
                             const Camera2D& cam,
                             double worldX, double worldY, double radius) {
        int cx, cy;
        cam.worldToScreen(worldX, worldY, cx, cy);
        int r = cam.scaleLength(radius);

        // Círculo de consulta en coral, dibujado con segmentos.
        SDL_SetRenderDrawColor(ren, 216, 90, 48, 255);
        const int SEGMENTS = 64;
        double prevX = cx + r, prevY = cy;
        for (int i = 1; i <= SEGMENTS; ++i) {
            double ang = 2.0 * M_PI * i / SEGMENTS;
            double x = cx + r * SDL_cos(ang);
            double y = cy + r * SDL_sin(ang);
            SDL_RenderDrawLine(ren,
                               static_cast<int>(prevX), static_cast<int>(prevY),
                               static_cast<int>(x), static_cast<int>(y));
            prevX = x;
            prevY = y;
        }

        // Marcar el centro.
        SDL_RenderDrawLine(ren, cx - 4, cy, cx + 4, cy);
        SDL_RenderDrawLine(ren, cx, cy - 4, cx, cy + 4);
    }

    void UI::updateHud(SDL_Window* window,
                       const FrameStats& fs,
                       double bruteForceMs,
                       int64_t bruteForceComparisons) {
        char title[512];
        std::snprintf(title, sizeof(title),
            "QuadTree Simulator | frame %.2f ms (QT)  vs  %.2f ms (BF) | "
            "comp QT=%lld BF=%lld | cand=%lld | colisiones=%d | nodos=%d",
            fs.totalMs, bruteForceMs,
            static_cast<long long>(fs.buildComparisons + fs.queryComparisons),
            static_cast<long long>(bruteForceComparisons),
            static_cast<long long>(fs.candidates),
            fs.collisions, fs.nodes);
        SDL_SetWindowTitle(window, title);
    }

} // namespace qt