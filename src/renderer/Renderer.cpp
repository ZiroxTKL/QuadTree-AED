#include "renderer/Renderer.hpp"
#include "renderer/DrawParticles.hpp"
#include "renderer/DrawQuadTree.hpp"
#include "renderer/UI.hpp"
#include "benchmark/BruteForce.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"

#include <SDL2/SDL.h>
#include <unordered_set>

namespace qt {

Renderer::Renderer(const Config& cfg)
    : cfg_(cfg), sim_(cfg) {
    camera_ = std::make_unique<Camera2D>(screenW_, screenH_, cfg_.width, cfg_.height);
}

Renderer::~Renderer() {
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_)   SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logError(std::string("SDL_Init fallo: ") + SDL_GetError());
        return false;
    }
    window_ = SDL_CreateWindow("QuadTree Simulator",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screenW_, screenH_, SDL_WINDOW_SHOWN);
    if (!window_) {
        logError(std::string("SDL_CreateWindow fallo: ") + SDL_GetError());
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1,
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        logError(std::string("SDL_CreateRenderer fallo: ") + SDL_GetError());
        return false;
    }
    return true;
}

void Renderer::handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false;      break;
                    case SDLK_SPACE:  paused_   = !paused_; break;
                    case SDLK_t:      showTree_ = !showTree_; break;
                    case SDLK_LEFT:   camera_->pan(-cfg_.width * 0.05, 0); break;
                    case SDLK_RIGHT:  camera_->pan( cfg_.width * 0.05, 0); break;
                    case SDLK_UP:     camera_->pan(0, -cfg_.height * 0.05); break;
                    case SDLK_DOWN:   camera_->pan(0,  cfg_.height * 0.05); break;
                    default: break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    // Fijar el punto de consulta de vecindad.
                    camera_->screenToWorld(e.button.x, e.button.y, queryX_, queryY_);
                    hasQuery_ = true;
                }
                break;
            case SDL_MOUSEWHEEL: {
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                double factor = e.wheel.y > 0 ? 1.1 : (1.0 / 1.1);
                camera_->zoomAt(mx, my, factor);
                break;
            }
            default:
                break;
        }
    }
}

void Renderer::update() {
    if (paused_) return;
    lastStats_ = sim_.stepFrame();
}

void Renderer::render() {
    // Fondo oscuro.
    SDL_SetRenderDrawColor(renderer_, 18, 18, 22, 255);
    SDL_RenderClear(renderer_);

    // 1) Subdivisiones del QuadTree.
    if (showTree_) {
        DrawQuadTree::draw(renderer_, *camera_, sim_.tree());
    }

    // 2) Si hay una consulta activa, calcular candidatos y vecinos.
    std::unordered_set<int> candidateIds;
    if (hasQuery_) {
        int64_t cmp = 0;
        auto neighbors = sim_.tree().queryNeighbors(queryX_, queryY_,
                                                    cfg_.queryRadius, cmp);
        for (const Particle* p : neighbors) candidateIds.insert(p->id);
    }

    // 3) Partículas + colisiones.
    DrawParticles::draw(renderer_, *camera_, sim_.particles(), sim_.lastCollisions());

    // 4) Candidatos resaltados y círculo de consulta.
    if (hasQuery_) {
        DrawParticles::drawCandidates(renderer_, *camera_, sim_.particles(), candidateIds);
        UI::drawQueryCircle(renderer_, *camera_, queryX_, queryY_, cfg_.queryRadius);
    }

    // 5) HUD: comparar contra fuerza bruta para el mismo frame.
    Timer bt;
    CollisionResult bf = BruteForce::detectCollisions(sim_.particles());
    double bfMs = bt.elapsedMs();
    UI::updateHud(window_, lastStats_, bfMs, bf.comparisons);

    SDL_RenderPresent(renderer_);
}

void Renderer::run() {
    if (!init()) {
        logError("No se pudo inicializar el renderer. Abortando.");
        return;
    }
    camera_->fitToWorld(cfg_.width, cfg_.height);

    bool running = true;
    while (running) {
        handleEvents(running);
        update();
        render();
    }
}

} // namespace qt
