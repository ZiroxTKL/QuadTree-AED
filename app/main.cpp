#include "raylib.h"
#include "Renderer.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "Distribution.h"
#include "Metrics.h"
#include <algorithm>
#include <vector>

static constexpr float WORLD_W = 1000.f;
static constexpr float WORLD_H = 1000.f;
static constexpr int WIN_W = 1280;
static constexpr int WIN_H = 900;
static constexpr int N_PARTICLES = 600;
static constexpr int QT_CAPACITY = 8; // máximo de elementos por nodo QT antes de dividirlo
static constexpr int QT_MAX_DEPTH = 16;

// BF se ejecuta cada cuadro solo cuando la cantidad de partículas es menor o igual a este umbral.
static constexpr int BF_LIVE_THRESHOLD = 3000;

int main()
{
    // ---- muestra la ventana ----
    Renderer renderer(WIN_W, WIN_H, WORLD_W, WORLD_H);

    // ---- simulacion ----
    Simulation sim(WORLD_W, WORLD_H, QT_CAPACITY);

    auto particles = dist::generate(
        dist::Kind::Uniform,
        N_PARTICLES,
        WORLD_W, WORLD_H,
        3.f, 6.f, // radio mínimo / máximo
        80.f,     // velocidad máxima
        42u       // semilla RNG — cambia para otro diseño
    );
    sim.setParticles(std::move(particles));

    bool paused = false;

    // ---- Bucle principal ----
    while (!renderer.windowShouldClose())
    {

        // 1. Entrada (modos de consulta, alternar vistas, etc.)
        renderer.handleInput(sim);

        // SPACE alterna la pausa de la simulación
        if (IsKeyPressed(KEY_SPACE))
            paused = !paused;

        // 2. Paso de física + reconstrucción del árbol (se omite si está en pausa)
        if (!paused)
        {
            float dt = GetFrameTime();
            dt = std::min(dt, 1.f / 30.f); // límite para evitar un efecto de espiral a bajo FPS

            sim.step(dt);
            sim.rebuildTree();
        }

        // 3. Detección de colisiones — ruta con QuadTree
        Metrics qtM, bfM;
        auto collisions = sim.detectCollisionsQuadTree(qtM);

        // 4. Detección de colisiones — ruta de Fuerza bruta (para comparación en el HUD)
        if ((int)sim.particles().size() <= BF_LIVE_THRESHOLD)
            brute::detectCollisions(sim.particles(), bfM);
        // en caso contrario, bfM sigue en cero; el renderizador mostrará la estimación teórica

        // 5. Renderizado
        renderer.draw(sim, collisions, qtM, bfM, paused);
    }

    return 0;
}
