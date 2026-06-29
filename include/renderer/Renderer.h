#pragma once
#include "core/Config.hpp"
#include "simulation/Simulation.hpp"
#include "renderer/Camera2D.hpp"

#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace qt {

    // Ventana de visualización interactiva basada en SDL2.
    // Muestra las partículas, las subdivisiones del QuadTree, la región
    // consultada, los candidatos, las colisiones y las métricas por frame.
    //
    // Controles:
    //   ESC / cerrar ventana  -> salir
    //   Espacio               -> pausar / reanudar
    //   T                     -> mostrar / ocultar el QuadTree
    //   Click izquierdo       -> fijar el punto de consulta de vecindad
    //   Rueda del ratón       -> zoom
    //   Flechas               -> desplazar la cámara
    class Renderer {
    public:
        explicit Renderer(const Config& cfg);
        ~Renderer();

        // Ejecuta el loop principal hasta que el usuario cierre la ventana.
        void run();

    private:
        bool init();
        void handleEvents(bool& running);
        void update();
        void render();

        Config cfg_;
        Simulation sim_;
        std::unique_ptr<Camera2D> camera_;

        SDL_Window*   window_   = nullptr;
        SDL_Renderer* renderer_ = nullptr;

        int  screenW_ = 900;
        int  screenH_ = 900;
        bool paused_      = false;
        bool showTree_    = true;
        bool hasQuery_    = false;
        double queryX_    = 0.0;
        double queryY_    = 0.0;

        FrameStats lastStats_;
    };

} // namespace qt
