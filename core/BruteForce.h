#pragma once
#include <vector>
#include <utility>
#include "Particle.h"
#include "Metrics.h"

namespace brute {
    vector<pair<int,int>> detectCollisions(
        const vector<Particle>& parts, Metrics& m);
}