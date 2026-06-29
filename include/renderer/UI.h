#pragma once
#include "renderer/Camera2D.hpp"
#include "simulation/Simulation.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace qt {

    // Dibuja elementos de interfaz: el círculo de la consulta de vecindad
    // activa y un resumen de estadísticas. El texto de estadísticas se
    // muestra en el título de la ventana (evita depender de SDL_ttf y fuentes).
    class UI {
    public:
        // Dibuja el círculo del radio de vecindad alrededor de un punto del mundo.
        static void drawQueryCircle(SDL_Renderer* ren,
                                    const Camera2D& cam,
                                    double worldX, double worldY, double radius);

        // Actualiza el título de la ventana con las métricas del frame y la
        // comparación contra fuerza bruta.
        static void updateHud(SDL_Window* window,
                              const FrameStats& fs,
                              double bruteForceMs,
                              int64_t bruteForceComparisons);
    };

} // namespace qt
