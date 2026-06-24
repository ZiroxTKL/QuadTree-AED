#include "BruteForce.h"

namespace brute {

vector<pair<int,int>> detectCollisions(const vector<Particle>& parts, Metrics& m) {
    vector<pair<int,int>> pairs;
    const int n = (int)parts.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            m.comparisons++;
            const Particle& a = parts[i];
            const Particle& b = parts[j];
            double dx = a.x - b.x, dy = a.y - b.y;
            double rr = a.radius + b.radius;
            if (dx * dx + dy * dy <= rr * rr)
                pairs.push_back({ a.id, b.id });
        }
    }
    return pairs;
}

}