#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <utility>
#include <string>
#include <cstdlib>
#include <filesystem>
#include "Simulation.h"
#include "BruteForce.h"
#include "Distribution.h"

using namespace std;
using namespace std::chrono;

static double ms(high_resolution_clock::time_point a, high_resolution_clock::time_point b) {
    return duration<double, milli>(b - a).count();
}

struct Avg {
    double build_ms      = 0;
    double qt_query_ms   = 0;
    double qt_comp       = 0;
    double qt_nodos      = 0;
    double qt_candidatos = 0;
    double bf_query_ms   = 0;
    double bf_comp       = 0;
    double colisiones    = 0;
};

int main(int argc, char** argv) {
    int repeats = 5;
    if (argc > 1) repeats = max(1, atoi(argv[1]));
    std::filesystem::create_directories("docs");
    string csvPath = (argc > 2) ? argv[2] : "docs/benchmark_resultados.csv";

    const float W = 1000, H = 1000;
    const int   capacity = 8;

    int sizes[] = { 1000, 5000, 10000 };
    dist::Kind kinds[] = { dist::Kind::Uniform, dist::Kind::Clusters, dist::Kind::HighDensity };
    const char* names[] = { "uniforme", "clusters", "alta_densidad" };

    ofstream csv(csvPath);
    if (!csv) {
        cerr << "No se pudo abrir el archivo de salida: " << csvPath << "\n";
        return 1;
    }
    csv << "n,distribucion,repeticiones,build_ms,qt_query_ms,qt_comparaciones,qt_nodos,"
           "qt_candidatos_por_obj,bf_query_ms,bf_comparaciones,"
           "speedup_comparaciones,speedup_tiempo,colisiones\n";
    csv << fixed << setprecision(3);

    cout << fixed << setprecision(3);
    cout << "\n=== QuadTree vs Fuerza Bruta  (promedio de " << repeats
         << " corridas, capacidad de nodo = " << capacity << ", mundo "
         << (int)W << "x" << (int)H << ") ===\n\n";

    {
        auto wp = dist::generate(dist::Kind::Uniform, 2000, W, H, 2.0f, 4.0f, 60.0f, 1u);
        Simulation ws(W, H, capacity);
        ws.setParticles(move(wp));
        ws.rebuildTree();
        Metrics wm;
        volatile size_t s = ws.detectCollisionsQuadTree(wm).size();
        s += brute::detectCollisions(ws.particles(), wm).size();
        (void)s;
    }

    for (int n : sizes) {
        for (int k = 0; k < 3; ++k) {
            Avg acc;

            for (int r = 0; r < repeats; ++r) {
                unsigned seed = 42u + (unsigned)r;   // semilla distinta por repeticion
                auto parts = dist::generate(kinds[k], n, W, H, 2.0f, 4.0f, 60.0f, seed);

                Simulation sim(W, H, capacity);
                sim.setParticles(move(parts));

                auto t0 = high_resolution_clock::now();
                sim.rebuildTree();
                auto t1 = high_resolution_clock::now();

                Metrics qtM, bfM;

                auto a = high_resolution_clock::now();
                auto collQT = sim.detectCollisionsQuadTree(qtM);
                auto b = high_resolution_clock::now();

                auto collBF = brute::detectCollisions(sim.particles(), bfM);
                auto c = high_resolution_clock::now();

                acc.build_ms      += ms(t0, t1);
                acc.qt_query_ms   += ms(a, b);
                acc.qt_comp       += (double)qtM.comparisons;
                acc.qt_nodos      += (double)qtM.nodesVisited;
                acc.qt_candidatos += (double)qtM.candidates;
                acc.bf_query_ms   += ms(b, c);
                acc.bf_comp       += (double)bfM.comparisons;
                acc.colisiones    += (double)collQT.size();

                if (collQT.size() != collBF.size())
                    cerr << "  [ADVERTENCIA] discrepancia QT(" << collQT.size()
                         << ") vs BF(" << collBF.size() << ") en n=" << n
                         << " " << names[k] << "\n";
            }

            double inv = 1.0 / repeats;
            double build_ms = acc.build_ms      * inv;
            double qt_query = acc.qt_query_ms   * inv;
            double qt_comp  = acc.qt_comp       * inv;
            double qt_nodos = acc.qt_nodos      * inv;
            double qt_cand  = acc.qt_candidatos * inv;
            double bf_query = acc.bf_query_ms   * inv;
            double bf_comp  = acc.bf_comp       * inv;
            double colis    = acc.colisiones    * inv;
            double cand_obj = qt_cand / (double)n;
            double sp_comp  = (qt_comp  > 0) ? bf_comp  / qt_comp  : 0.0;
            double sp_time  = (qt_query > 0) ? bf_query / qt_query : 0.0;

            csv << n << "," << names[k] << "," << repeats << ","
                << build_ms << "," << qt_query << "," << (long long)qt_comp << ","
                << (long long)qt_nodos << "," << cand_obj << ","
                << bf_query << "," << (long long)bf_comp << ","
                << sp_comp << "," << sp_time << "," << (long long)(colis + 0.5) << "\n";

            cout << "n=" << setw(6) << n << "  " << setw(14) << left << names[k] << right
                 << " | build " << setw(8) << build_ms << " ms"
                 << " | QT " << setw(9) << qt_query << " ms"
                 << " | BF " << setw(10) << bf_query << " ms"
                 << " | comp QT " << setw(11) << (long long)qt_comp
                 << " vs BF " << setw(11) << (long long)bf_comp
                 << " | cand/obj " << setw(7) << cand_obj
                 << " | speedup x" << setw(7) << sp_comp << " comp / x"
                 << setw(6) << sp_time << " t"
                 << " | colis " << (long long)(colis + 0.5) << "\n";
        }
        cout << "\n";
    }

    csv.close();
    cout << "CSV guardado en: "
     << std::filesystem::absolute(csvPath)
     << "\n";
    return 0;
}
