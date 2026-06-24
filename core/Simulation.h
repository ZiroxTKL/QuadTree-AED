#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "Particle.h"
#include "QuadTree.h"
#include "Metrics.h"

class Simulation {
public:
    Simulation(float worldW, float worldH, int capacity);

    void setParticles(vector<Particle> ps);
    void step(float dt);
    void rebuildTree();

    vector<pair<int,int>> detectCollisionsQuadTree(Metrics& m);

    const vector<Particle>& particles() const {
         return parts; 
    }

    const QuadTree& tree() const {
         return *qt; 
    }

private:
    float W, H;
    int   capacity;
    vector<Particle> parts;
    unique_ptr<QuadTree> qt;
};