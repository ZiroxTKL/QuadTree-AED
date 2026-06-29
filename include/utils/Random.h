#pragma once
#include <random>
#include <cstdint>

// Generador de aleatorios con semilla fija para reproducibilidad.
// Envuelve std::mt19937 para que toda la app use la misma fuente.
namespace qt {

    class Random {
    public:
        explicit Random(uint64_t seed = 42) : engine_(seed) {}

        void reseed(uint64_t seed) { engine_.seed(seed); }

        // Real uniforme en [a, b)
        double uniform(double a, double b) {
            std::uniform_real_distribution<double> dist(a, b);
            return dist(engine_);
        }

        // Entero uniforme en [a, b]
        int uniformInt(int a, int b) {
            std::uniform_int_distribution<int> dist(a, b);
            return dist(engine_);
        }

        // Normal (gaussiana) con media y desviación dadas
        double normal(double mean, double stddev) {
            std::normal_distribution<double> dist(mean, stddev);
            return dist(engine_);
        }

    private:
        std::mt19937_64 engine_;
    };

} // namespace qt
