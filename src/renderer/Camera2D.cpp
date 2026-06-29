#include "renderer/Camera2D.hpp"
#include <algorithm>

namespace qt {

Camera2D::Camera2D(int screenW, int screenH, double worldW, double worldH)
    : screenW_(screenW), screenH_(screenH) {
    fitToWorld(worldW, worldH);
}

void Camera2D::fitToWorld(double worldW, double worldH) {
    double sx = screenW_ / worldW;
    double sy = screenH_ / worldH;
    scale_ = std::min(sx, sy) * 0.95;  // pequeño margen
    // Centrar el mundo en la pantalla.
    offsetX_ = (worldW - screenW_ / scale_) * 0.5;
    offsetY_ = (worldH - screenH_ / scale_) * 0.5;
}

void Camera2D::worldToScreen(double wx, double wy, int& sx, int& sy) const {
    sx = static_cast<int>((wx - offsetX_) * scale_);
    sy = static_cast<int>((wy - offsetY_) * scale_);
}

void Camera2D::screenToWorld(int sx, int sy, double& wx, double& wy) const {
    wx = sx / scale_ + offsetX_;
    wy = sy / scale_ + offsetY_;
}

int Camera2D::scaleLength(double worldLen) const {
    int v = static_cast<int>(worldLen * scale_);
    return v < 1 ? 1 : v;
}

void Camera2D::pan(double dxWorld, double dyWorld) {
    offsetX_ += dxWorld;
    offsetY_ += dyWorld;
}

void Camera2D::zoomAt(int sx, int sy, double factor) {
    // Mantener fijo el punto del mundo bajo el cursor al hacer zoom.
    double wx, wy;
    screenToWorld(sx, sy, wx, wy);
    scale_ *= factor;
    double nx, ny;
    screenToWorld(sx, sy, nx, ny);
    offsetX_ += wx - nx;
    offsetY_ += wy - ny;
}

void Camera2D::resize(int screenW, int screenH) {
    screenW_ = screenW;
    screenH_ = screenH;
}

} // namespace qt
