#include "Distribution.h"
#include "Vec2.h"
#include <random>
#include <algorithm>

namespace dist {

static double clampd(double v, double lo, double hi) {
    return max(lo, min(v, hi));
}

    vector<Particle> generate(Kind kind, int n, float worldW, float worldH,
                               float minRadius, float maxRadius,
                               float maxSpeed, unsigned seed) {
    mt19937 rng(seed);
    uniform_real_distribution<double> ux(0.0, worldW);
    uniform_real_distribution<double> uy(0.0, worldH);
    uniform_real_distribution<double> uvel(-maxSpeed, maxSpeed);
    uniform_real_distribution<double> urad(minRadius, maxRadius);

    const int numClusters = 5;
    vector<Vec2> centers;
    for (int i = 0; i < numClusters; ++i)
        centers.push_back({ (float)ux(rng), (float)uy(rng) });
    normal_distribution<double> spread(0.0,min(worldW, worldH) * 0.05);
    uniform_int_distribution<int> pick(0, numClusters - 1);

    Vec2 hot = { worldW * 0.5f, worldH * 0.5f };
    normal_distribution<double> hotSpread(0.0, min(worldW, worldH) * 0.02);
    bernoulli_distribution inHot(0.8);

    vector<Particle> out;
    out.reserve(n);

    for (int i = 0; i < n; ++i) {
        double x = 0, y = 0;
        switch (kind) {
            case Kind::Uniform:
                x = ux(rng); y = uy(rng);
                break;
            case Kind::Clusters: {
                Vec2 c = centers[pick(rng)];
                x = c.x + spread(rng);
                y = c.y + spread(rng);
                break;
            }
            case Kind::HighDensity:
                if (inHot(rng)) { x = hot.x + hotSpread(rng); y = hot.y + hotSpread(rng); }
                else            { x = ux(rng); y = uy(rng); }
                break;
        }
        Particle p;
        p.id = i;
        p.x = clampd(x, 0.0, worldW);
        p.y = clampd(y, 0.0, worldH);
        p.vx = uvel(rng);
        p.vy = uvel(rng);
        p.radius = urad(rng);
        out.push_back(p);
    }
    return out;
}

}