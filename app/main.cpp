#include "raylib.h"
#include "Renderer.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "Distribution.h"
#include "Metrics.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstdio>

struct Config {
    float worldW   = 1000.f;
    float worldH   = 1000.f;
    int   winW     = 1280;
    int   winH     = 900;
    int   n        = 600;
    int   capacity = 8;      // maximo de elementos por nodo del QuadTree antes de subdividir
    float rmin     = 3.f;    // radio minimo
    float rmax     = 6.f;    // radio maximo
    float speed    = 80.f;   // velocidad maxima
    unsigned seed  = 42u;    // semilla del generador
    dist::Kind kind = dist::Kind::Uniform;
};

static constexpr int BF_LIVE_THRESHOLD = 3000;

static const char* kindName(dist::Kind k) {
    switch (k) {
        case dist::Kind::Uniform:     return "Uniforme";
        case dist::Kind::Clusters:    return "Clusters";
        case dist::Kind::HighDensity: return "Alta densidad";
    }
    return "?";
}

static void printUsage(const char* exe) {
    std::printf(
        "Simulador de particulas 2D con QuadTree\n"
        "Uso: %s [opciones]\n"
        "  --n <int>        numero de particulas            (def 600)\n"
        "  --world <float>  tamano del mundo 2D (cuadrado)  (def 1000)\n"
        "  --cap <int>      capacidad maxima por nodo QT     (def 8)\n"
        "  --rmin <float>   radio minimo                     (def 3)\n"
        "  --rmax <float>   radio maximo                     (def 6)\n"
        "  --speed <float>  velocidad maxima                 (def 80)\n"
        "  --seed <uint>    semilla del generador            (def 42)\n"
        "  --dist <nombre>  uniforme | clusters | alta_densidad (def uniforme)\n"
        "  --win <w> <h>    tamano de la ventana             (def 1280 900)\n"
        "  -h, --help       muestra esta ayuda y termina\n",
        exe);
}

static bool parseArgs(int argc, char** argv, Config& c) {
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto nextf = [&](float& out) { if (i + 1 < argc) out = (float)std::atof(argv[++i]); };
        auto nexti = [&](int& out)   { if (i + 1 < argc) out = std::atoi(argv[++i]); };

        if      (a == "--n")                         nexti(c.n);
        else if (a == "--world")                   { nextf(c.worldW); c.worldH = c.worldW; }
        else if (a == "--cap" || a == "--capacity")  nexti(c.capacity);
        else if (a == "--rmin")                      nextf(c.rmin);
        else if (a == "--rmax")                      nextf(c.rmax);
        else if (a == "--speed")                     nextf(c.speed);
        else if (a == "--seed")                    { if (i + 1 < argc) c.seed = (unsigned)std::strtoul(argv[++i], nullptr, 10); }
        else if (a == "--dist") {
            if (i + 1 < argc) {
                std::string d = argv[++i];
                if      (d == "clusters")                              c.kind = dist::Kind::Clusters;
                else if (d == "alta_densidad" || d == "highdensity")   c.kind = dist::Kind::HighDensity;
                else                                                   c.kind = dist::Kind::Uniform;
            }
        }
        else if (a == "--win")                     { nexti(c.winW); nexti(c.winH); }
        else if (a == "-h" || a == "--help")       { printUsage(argv[0]); return false; }
        else { std::printf("Opcion desconocida: %s\n", a.c_str()); printUsage(argv[0]); return false; }
    }
    // saneamiento
    c.n        = std::max(1, c.n);
    c.capacity = std::max(1, c.capacity);
    if (c.rmax < c.rmin) std::swap(c.rmin, c.rmax);
    return true;
}

int main(int argc, char** argv)
{
    Config cfg;
    if (!parseArgs(argc, argv, cfg)) return 0;

    Renderer renderer(cfg.winW, cfg.winH, cfg.worldW, cfg.worldH);

    auto sim = std::make_unique<Simulation>(cfg.worldW, cfg.worldH, cfg.capacity);

    auto regenerate = [&](bool newSim) {
        if (newSim)
            sim = std::make_unique<Simulation>(cfg.worldW, cfg.worldH, cfg.capacity);
        auto particles = dist::generate(cfg.kind, cfg.n, cfg.worldW, cfg.worldH,
                                        cfg.rmin, cfg.rmax, cfg.speed, cfg.seed);
        sim->setParticles(std::move(particles));
        renderer.setConfig(kindName(cfg.kind), cfg.n, cfg.capacity, cfg.seed);
    };
    regenerate(false);

    bool paused = false;

    while (!renderer.windowShouldClose())
    {
        renderer.handleInput(*sim);

        if (IsKeyPressed(KEY_SPACE)) paused = !paused;

        if (IsKeyPressed(KEY_ONE))   { cfg.kind = dist::Kind::Uniform;     regenerate(false); }
        if (IsKeyPressed(KEY_TWO))   { cfg.kind = dist::Kind::Clusters;    regenerate(false); }
        if (IsKeyPressed(KEY_THREE)) { cfg.kind = dist::Kind::HighDensity; regenerate(false); }
        if (IsKeyPressed(KEY_G))     { cfg.seed += 1; regenerate(false); }
        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
            { cfg.n += 100;                     regenerate(false); }
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
            { cfg.n = std::max(1, cfg.n - 100); regenerate(false); }
        if (IsKeyPressed(KEY_LEFT_BRACKET))  { cfg.capacity = std::max(1, cfg.capacity - 1); regenerate(true); }
        if (IsKeyPressed(KEY_RIGHT_BRACKET)) { cfg.capacity += 1;                            regenerate(true); }

        if (!paused)
        {
            float dt = GetFrameTime();
            dt = std::min(dt, 1.f / 30.f);
            sim->step(dt);
            sim->rebuildTree();
        }

        Metrics qtM, bfM;
        auto collisions = sim->detectCollisionsQuadTree(qtM);

        if ((int)sim->particles().size() <= BF_LIVE_THRESHOLD)
            brute::detectCollisions(sim->particles(), bfM);

        renderer.draw(*sim, collisions, qtM, bfM, paused);
    }

    return 0;
}
