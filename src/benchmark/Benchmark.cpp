#include "benchmark/Benchmark.hpp"
#include "benchmark/BruteForce.hpp"
#include "benchmark/Statistics.hpp"
#include "simulation/Simulation.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

namespace qt {

BenchmarkRow Benchmark::run(int n) {
    Config cfg = base_;
    cfg.n = n;

    BenchmarkRow row;
    row.n = n;
    row.distribution = distributionToString(cfg.distribution);

    Simulation sim(cfg);

    Statistics qtFrame, qtCmp, qtCand;
    Statistics bfFrame, bfCmp;

    bool everMismatched = false;

    for (int f = 0; f < cfg.frames; ++f) {
        // --- Paso del QuadTree (mover + reconstruir + colisiones) ---
        FrameStats fs = sim.stepFrame();
        qtFrame.add(fs.totalMs);
        qtCmp.add(static_cast<double>(fs.buildComparisons + fs.queryComparisons));
        qtCand.add(static_cast<double>(fs.candidates));

        // Pares detectados por el QuadTree en este frame.
        std::set<std::pair<int,int>> qtPairs;
        for (const auto& cp : sim.lastCollisions()) {
            qtPairs.insert({std::min(cp.a, cp.b), std::max(cp.a, cp.b)});
        }

        // --- Fuerza bruta sobre el mismo estado de partículas ---
        Timer bt;
        CollisionResult bf = BruteForce::detectCollisions(sim.particles());
        bfFrame.add(bt.elapsedMs());
        bfCmp.add(static_cast<double>(bf.comparisons));

        // --- Validación: comparar conjuntos de pares ---
        std::set<std::pair<int,int>> bfPairs;
        for (const auto& cp : bf.pairs) {
            bfPairs.insert({std::min(cp.a, cp.b), std::max(cp.a, cp.b)});
        }
        if (qtPairs != bfPairs) {
            everMismatched = true;
        }
    }

    row.qtAvgFrameMs    = qtFrame.mean();
    row.qtAvgComparisons = qtCmp.mean();
    row.qtAvgCandidates  = qtCand.mean();
    row.qtCandidatesPerObject = n > 0 ? qtCand.mean() / n : 0.0;

    row.bfAvgFrameMs    = bfFrame.mean();
    row.bfAvgComparisons = bfCmp.mean();

    row.resultsMatch = !everMismatched;
    row.speedup = row.qtAvgFrameMs > 0.0 ? row.bfAvgFrameMs / row.qtAvgFrameMs : 0.0;

    return row;
}

std::vector<BenchmarkRow> Benchmark::runAll(const std::vector<int>& sizes,
                                            const std::string& outDir) {
    std::vector<BenchmarkRow> rows;

    for (int n : sizes) {
        std::ostringstream msg;
        msg << "Benchmark n=" << n << " (" << distributionToString(base_.distribution)
            << ", " << base_.frames << " frames)...";
        logInfo(msg.str());

        BenchmarkRow row = run(n);
        rows.push_back(row);

        // CSV individual por tamaño: una fila con métricas detalladas.
        std::ostringstream path;
        path << outDir << "/benchmark_" << n << ".csv";
        std::ofstream out(path.str());
        if (out.is_open()) {
            out << "metric,quadtree,brute_force\n";
            out << "n," << n << "," << n << "\n";
            out << "distribution," << row.distribution << "," << row.distribution << "\n";
            out << "avg_frame_ms," << row.qtAvgFrameMs << "," << row.bfAvgFrameMs << "\n";
            out << "avg_comparisons," << row.qtAvgComparisons << "," << row.bfAvgComparisons << "\n";
            out << "avg_candidates," << row.qtAvgCandidates << ",\n";
            out << "candidates_per_object," << row.qtCandidatesPerObject << ",\n";
            out << "speedup," << row.speedup << ",\n";
            out << "results_match," << (row.resultsMatch ? "true" : "false") << ",\n";
            out.close();
            logInfo("  -> " + path.str());
        } else {
            logWarn("No se pudo escribir " + path.str());
        }
    }

    return rows;
}

void Benchmark::writeSummaryCsv(const std::vector<BenchmarkRow>& rows,
                                const std::string& path) {
    std::ofstream out(path);
    if (!out.is_open()) {
        logWarn("No se pudo escribir el resumen " + path);
        return;
    }
    out << "n,distribution,qt_frame_ms,bf_frame_ms,qt_comparisons,bf_comparisons,"
           "qt_candidates,candidates_per_object,speedup,results_match\n";
    for (const auto& r : rows) {
        out << r.n << ","
            << r.distribution << ","
            << r.qtAvgFrameMs << ","
            << r.bfAvgFrameMs << ","
            << r.qtAvgComparisons << ","
            << r.bfAvgComparisons << ","
            << r.qtAvgCandidates << ","
            << r.qtCandidatesPerObject << ","
            << r.speedup << ","
            << (r.resultsMatch ? "true" : "false") << "\n";
    }
    out.close();
    logInfo("Resumen escrito en " + path);
}

} // namespace qt
