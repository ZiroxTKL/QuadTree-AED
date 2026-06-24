#pragma once
#include <vector>
#include "Particle.h"

namespace dist {
    enum class Kind { Uniform, Clusters, HighDensity };

    vector<Particle> generate(Kind kind, int n, float worldW, float worldH, float minRadius, float maxRadius, float maxSpeed, unsigned seed = 42);
}
