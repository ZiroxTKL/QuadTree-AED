#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <utility>
#include "Simulation.h"
#include "BruteForce.h"
#include "Distribution.h"

using namespace std;
using namespace std::chrono;

static double ms(high_resolution_clock::time_point a, high_resolution_clock::time_point b) {
    return duration<double, milli>(b - a).count();
}

int main() {
    const float W = 1000, H = 1000;
    int sizes[] = { 1000, 5000, 10000 };
    dist::Kind kinds[] = { dist::Kind::Uniform, dist::Kind::Clusters, dist::Kind::HighDensity };
    const char* names[] = { "uniforme", "clusters", "alta_densidad" };

    cout << "n,distribucion,build_ms,qt_query_ms,qt_comparaciones,qt_nodos,"
            "bf_query_ms,bf_comparaciones,colisiones_qt,colisiones_bf\n";
    cout << fixed << setprecision(3);

    for (int n : sizes) {
        for (int k = 0; k < 3; ++k) {
            auto parts = dist::generate(kinds[k], n, W, H, 2.0f, 4.0f, 60.0f);

            Simulation sim(W, H, 8);
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

            cout << n << "," << names[k] << ","
                 << ms(t0, t1) << ","
                 << ms(a, b) << "," << qtM.comparisons << "," << qtM.nodesVisited << ","
                 << ms(b, c) << "," << bfM.comparisons << ","
                 << collQT.size() << "," << collBF.size() << "\n";
        }
    }
    return 0;
}