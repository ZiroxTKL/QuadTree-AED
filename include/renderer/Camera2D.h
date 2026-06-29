#pragma once

namespace qt {

    // Cámara 2D: convierte coordenadas del mundo (el espacio de la simulación)
    // a coordenadas de pantalla (píxeles), con desplazamiento y zoom.
    class Camera2D {
    public:
        Camera2D(int screenW, int screenH, double worldW, double worldH);

        // Ajusta el zoom para que todo el mundo quepa en la pantalla.
        void fitToWorld(double worldW, double worldH);

        void worldToScreen(double wx, double wy, int& sx, int& sy) const;
        void screenToWorld(int sx, int sy, double& wx, double& wy) const;

        // Convierte una longitud del mundo a píxeles (para radios).
        int scaleLength(double worldLen) const;

        void pan(double dxWorld, double dyWorld);
        void zoomAt(int sx, int sy, double factor);

        void resize(int screenW, int screenH);

    private:
        int screenW_, screenH_;
        double offsetX_ = 0.0, offsetY_ = 0.0;  // esquina del mundo visible
        double scale_ = 1.0;                     // píxeles por unidad de mundo
    };

} // namespace qt
