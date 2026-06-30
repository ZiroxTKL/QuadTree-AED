#include "raylib.h"
#include "Renderer.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "Distribution.h"
#include "Metrics.h"
#include <algorithm>
#include <memory>
#include <vector>

static constexpr int WIN_W = 1280;
static constexpr int WIN_H = 900;
static constexpr int BF_LIVE_THRESHOLD = 3000;

struct AppConfig {
    float worldW = 1000.f;
    float worldH = 1000.f;
    int particleCount = 1000;
    int capacity = 8;
    float minRadius = 3.f;
    float maxRadius = 6.f;
    float maxSpeed = 80.f;
    dist::Kind distribution = dist::Kind::HighDensity;
    unsigned seed = 42u;
};

static const char* distributionName(dist::Kind kind)
{
    switch (kind) {
        case dist::Kind::Uniform:     return "Uniforme";
        case dist::Kind::Clusters:    return "Clusters";
        case dist::Kind::HighDensity: return "Alta densidad";
    }
    return "Desconocida";
}

static void regenerateScenario(Renderer& renderer,
                               std::unique_ptr<Simulation>& sim,
                               const AppConfig& cfg)
{
    renderer.setWorldSize(cfg.worldW, cfg.worldH);
    renderer.setScenarioInfo(distributionName(cfg.distribution),
                             cfg.capacity,
                             cfg.minRadius,
                             cfg.maxRadius,
                             cfg.maxSpeed);

    sim = std::make_unique<Simulation>(cfg.worldW, cfg.worldH, cfg.capacity);
    auto particles = dist::generate(cfg.distribution,
                                    cfg.particleCount,
                                    cfg.worldW,
                                    cfg.worldH,
                                    cfg.minRadius,
                                    cfg.maxRadius,
                                    cfg.maxSpeed,
                                    cfg.seed);
    sim->setParticles(std::move(particles));
}

static void cycleParticleCount(AppConfig& cfg)
{
    if (cfg.particleCount == 1000) {
        cfg.particleCount = 5000;
    } else if (cfg.particleCount == 5000) {
        cfg.particleCount = 10000;
    } else {
        cfg.particleCount = 1000;
    }
}

static void cycleWorldSize(AppConfig& cfg)
{
    if (cfg.worldW == 1000.f) {
        cfg.worldW = cfg.worldH = 1500.f;
    } else if (cfg.worldW == 1500.f) {
        cfg.worldW = cfg.worldH = 500.f;
    } else {
        cfg.worldW = cfg.worldH = 1000.f;
    }
}

static void cycleCapacity(AppConfig& cfg)
{
    if (cfg.capacity == 4) {
        cfg.capacity = 8;
    } else if (cfg.capacity == 8) {
        cfg.capacity = 16;
    } else if (cfg.capacity == 16) {
        cfg.capacity = 32;
    } else {
        cfg.capacity = 4;
    }
}

static void cycleRadiusRange(AppConfig& cfg)
{
    if (cfg.minRadius == 2.f && cfg.maxRadius == 4.f) {
        cfg.minRadius = 3.f;
        cfg.maxRadius = 6.f;
    } else if (cfg.minRadius == 3.f && cfg.maxRadius == 6.f) {
        cfg.minRadius = 6.f;
        cfg.maxRadius = 10.f;
    } else {
        cfg.minRadius = 2.f;
        cfg.maxRadius = 4.f;
    }
}

static void cycleMaxSpeed(AppConfig& cfg)
{
    if (cfg.maxSpeed == 20.f) {
        cfg.maxSpeed = 50.f;
    } else if (cfg.maxSpeed == 50.f) {
        cfg.maxSpeed = 80.f;
    } else if (cfg.maxSpeed == 80.f) {
        cfg.maxSpeed = 120.f;
    } else {
        cfg.maxSpeed = 20.f;
    }
}

int main()
{
    AppConfig cfg;
    Renderer renderer(WIN_W, WIN_H, cfg.worldW, cfg.worldH);
    std::unique_ptr<Simulation> sim;
    regenerateScenario(renderer, sim, cfg);

    bool paused = false;

    while (!renderer.windowShouldClose())
    {
        bool regenerate = false;

        if (IsKeyPressed(KEY_ONE)) {
            cfg.distribution = dist::Kind::Uniform;
            regenerate = true;
        }
        if (IsKeyPressed(KEY_TWO)) {
            cfg.distribution = dist::Kind::Clusters;
            regenerate = true;
        }
        if (IsKeyPressed(KEY_THREE)) {
            cfg.distribution = dist::Kind::HighDensity;
            regenerate = true;
        }
        if (IsKeyPressed(KEY_N)) {
            cycleParticleCount(cfg);
            regenerate = true;
        }
        if (IsKeyPressed(KEY_M)) {
            cycleWorldSize(cfg);
            regenerate = true;
        }
        if (IsKeyPressed(KEY_K)) {
            cycleCapacity(cfg);
            regenerate = true;
        }
        if (IsKeyPressed(KEY_T)) {
            cycleRadiusRange(cfg);
            regenerate = true;
        }
        if (IsKeyPressed(KEY_V)) {
            cycleMaxSpeed(cfg);
            regenerate = true;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            cfg.seed++;
            regenerate = true;
        }

        if (regenerate) {
            regenerateScenario(renderer, sim, cfg);
        }

        renderer.handleInput(*sim);

        if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if (!paused)
        {
            float dt = GetFrameTime();
            dt = std::min(dt, 1.f / 30.f);

            sim->step(dt);
            sim->rebuildTree();
        }

        Metrics qtM, bfM;
        auto collisions = sim->detectCollisionsQuadTree(qtM);

        if ((int)sim->particles().size() <= BF_LIVE_THRESHOLD) {
            brute::detectCollisions(sim->particles(), bfM);
        }

        renderer.draw(*sim, collisions, qtM, bfM, paused);
    }

    return 0;
}
