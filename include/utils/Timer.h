#pragma once
#include <chrono>

// Cronómetro simple para medir tiempos por frame o por consulta.
namespace qt {

    class Timer {
    public:
        Timer() { reset(); }

        void reset() {
            start_ = Clock::now();
        }

        // Tiempo transcurrido desde reset() en milisegundos
        double elapsedMs() const {
            auto now = Clock::now();
            std::chrono::duration<double, std::milli> diff = now - start_;
            return diff.count();
        }

        // Tiempo transcurrido en microsegundos
        double elapsedUs() const {
            auto now = Clock::now();
            std::chrono::duration<double, std::micro> diff = now - start_;
            return diff.count();
        }

    private:
        using Clock = std::chrono::high_resolution_clock;
        std::chrono::time_point<Clock> start_;
    };

} // namespace qt
