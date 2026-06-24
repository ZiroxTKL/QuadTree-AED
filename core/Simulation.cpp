#include "Simulation.h"
#include <algorithm>

Simulation::Simulation(float worldW, float worldH, int cap): W(worldW), H(worldH), capacity(cap) {}

void Simulation::setParticles(vector<Particle> ps) {
    parts = move(ps);
    rebuildTree();
}

void Simulation::step(float dt) {
    for (auto& p : parts) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        if (p.x - p.radius < 0) {
            p.x = p.radius;     p.vx = -p.vx;
        }
        if (p.x + p.radius > W) {
            p.x = W - p.radius; p.vx = -p.vx;
        }
        if (p.y - p.radius < 0) {
            p.y = p.radius;     p.vy = -p.vy;
        }
        if (p.y + p.radius > H) {
             p.y = H - p.radius; p.vy = -p.vy; 
        }
    }
}

void Simulation::rebuildTree() {
    AABB world{ { W * 0.5f, H * 0.5f }, { W * 0.5f, H * 0.5f } };
    qt = make_unique<QuadTree>(world, capacity);
    for (const auto& p : parts)
        qt->insert({p.id, { (float)p.x, (float)p.y } });
}

vector<pair<int,int>> Simulation::detectCollisionsQuadTree(Metrics& m) {
    vector<pair<int,int>> pairs;
    double maxR = 0.0;
    for (const auto& p : parts) maxR = max(maxR, p.radius);

    vector<int> candidates;
    for (const auto& p : parts) {
        candidates.clear();
        float r = (float)(p.radius + maxR);
        qt->queryRadius({ (float)p.x, (float)p.y }, r, candidates, m);
        m.candidates += (long long)candidates.size();

        for (int id : candidates) {
            if (id <= p.id) continue;
            const Particle& q = parts[id];
            double dx = p.x - q.x, dy = p.y - q.y;
            double rr = p.radius + q.radius;
            if (dx * dx + dy * dy <= rr * rr)
                pairs.push_back({ p.id, id });
        }
    }
    return pairs;
}