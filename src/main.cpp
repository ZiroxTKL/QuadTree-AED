#include "core/Config.hpp"
#include "simulation/Simulation.hpp"
#include "benchmark/Benchmark.hpp"
#include "utils/Logger.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <cstring>

#ifdef WITH_RENDERER
#include "renderer/Renderer.hpp"
#endif

using namespace qt;

namespace {

void printUsage(const char* prog) {
    std::cout <<
"Uso: " << prog << " [opciones]\n"
"\n"
"Modos:\n"
"  --benchmark            Corre la comparacion QuadTree vs fuerza bruta.\n"
"  --simulate             Corre la simulacion (con render si esta compilado).\n"
"\n"
"Opciones:\n"
"  --preset <archivo>     Carga parametros desde un JSON (data/presets/...).\n"
"  --config <archivo>     Alias de --preset.\n"
"  --n <int>              Numero de objetos (sobreescribe el preset).\n"
"  --frames <int>         Frames a simular en modo benchmark.\n"
"  --capacity <int>       Capacidad maxima por nodo del QuadTree.\n"
"  --dist <uniform|clusters|dense>  Distribucion inicial.\n"
"  --out <dir>            Carpeta de salida para los CSV (default: results).\n"
"  --sizes <a,b,c>        Tamanos a probar en benchmark (default: 1000,5000,10000).\n"
"  --help                 Muestra esta ayuda.\n";
}

std::vector<int> parseSizes(const std::string& s) {
    std::vector<int> out;
    std::string cur;
    for (char c : s) {
        if (c == ',') { if (!cur.empty()) { out.push_back(std::stoi(cur)); cur.clear(); } }
        else cur += c;
    }
    if (!cur.empty()) out.push_back(std::stoi(cur));
    return out;
}

} // namespace

int main(int argc, char** argv) {
    Config cfg;
    bool benchmarkMode = false;
    bool simulateMode  = false;
    std::string outDir = "results";
    std::vector<int> sizes = {1000, 5000, 10000};

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto needValue = [&](const char* name) -> std::string {
            if (i + 1 >= argc) {
                logError(std::string("Falta valor para ") + name);
                std::exit(1);
            }
            return argv[++i];
        };

        if (arg == "--benchmark")      benchmarkMode = true;
        else if (arg == "--simulate")  simulateMode = true;
        else if (arg == "--preset" || arg == "--config") {
            std::string path = needValue(arg.c_str());
            if (!cfg.loadFromFile(path)) {
                logError("No se pudo cargar el preset; usando valores por defecto.");
            }
        }
        else if (arg == "--n")        cfg.n = std::stoi(needValue("--n"));
        else if (arg == "--frames")   cfg.frames = std::stoi(needValue("--frames"));
        else if (arg == "--capacity") cfg.capacity = std::stoi(needValue("--capacity"));
        else if (arg == "--dist")     cfg.distribution = distributionFromString(needValue("--dist"));
        else if (arg == "--out")      outDir = needValue("--out");
        else if (arg == "--sizes")    sizes = parseSizes(needValue("--sizes"));
        else if (arg == "--help" || arg == "-h") { printUsage(argv[0]); return 0; }
        else {
            logWarn("Argumento desconocido: " + arg);
        }
    }

    if (!benchmarkMode && !simulateMode) {
        // Por defecto: si no se especifica modo, mostrar ayuda.
        printUsage(argv[0]);
        return 0;
    }

    cfg.printSummary();

    if (benchmarkMode) {
        Benchmark bench(cfg);
        auto rows = bench.runAll(sizes, outDir);
        Benchmark::writeSummaryCsv(rows, outDir + "/benchmark.csv");

        // Tabla resumen por consola.
        std::cout << "\n=== Resumen ===\n";
        std::cout << "n\tdist\t\tQT ms\tBF ms\tspeedup\tcand/obj\tmatch\n";
        for (const auto& r : rows) {
            std::cout << r.n << "\t" << r.distribution << "\t"
                      << r.qtAvgFrameMs << "\t" << r.bfAvgFrameMs << "\t"
                      << r.speedup << "x\t" << r.qtCandidatesPerObject << "\t\t"
                      << (r.resultsMatch ? "OK" : "DIFF") << "\n";
        }
    }

    if (simulateMode) {
#ifdef WITH_RENDERER
        Renderer renderer(cfg);
        renderer.run();
#else
        logInfo("Compilado sin renderer (SDL2 no disponible). "
                "Ejecutando simulacion headless por unos frames.");
        Simulation sim(cfg);
        int frames = cfg.frames > 0 ? cfg.frames : 100;
        for (int f = 0; f < frames; ++f) {
            FrameStats fs = sim.stepFrame();
            if (f % 20 == 0) {
                std::cout << "frame " << f
                          << "  total=" << fs.totalMs << "ms"
                          << "  colisiones=" << fs.collisions
                          << "  nodos=" << fs.nodes << "\n";
            }
        }
#endif
    }

    return 0;
}
