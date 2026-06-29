#pragma once
#include "core/Config.hpp"
#include <string>
#include <vector>

namespace qt {

    // Resultado agregado de un benchmark para un tamaño n concreto.
    struct BenchmarkRow {
        int    n = 0;
        std::string distribution;

        // QuadTree
        double qtAvgFrameMs   = 0.0;
        double qtAvgComparisons = 0.0;  // construcción + queries
        double qtAvgCandidates  = 0.0;  // candidatos revisados por frame
        double qtCandidatesPerObject = 0.0;

        // Fuerza bruta
        double bfAvgFrameMs   = 0.0;
        double bfAvgComparisons = 0.0;

        // Validación: ¿coinciden los pares detectados por ambos métodos?
        bool   resultsMatch = true;

        double speedup = 0.0;  // bfAvgFrameMs / qtAvgFrameMs
    };

    // Ejecuta los experimentos comparando QuadTree contra fuerza bruta.
    class Benchmark {
    public:
        explicit Benchmark(const Config& base) : base_(base) {}

        // Corre el benchmark para un tamaño n dado durante 'base.frames' frames.
        BenchmarkRow run(int n);

        // Corre varios tamaños (p.ej. 1000, 5000, 10000) y guarda un CSV por cada
        // uno más un CSV resumen. 'outDir' es la carpeta de salida (results/).
        std::vector<BenchmarkRow> runAll(const std::vector<int>& sizes,
                                         const std::string& outDir);

        // Escribe el CSV resumen con todas las filas.
        static void writeSummaryCsv(const std::vector<BenchmarkRow>& rows,
                                    const std::string& path);

    private:
        Config base_;
    };

} // namespace qt
