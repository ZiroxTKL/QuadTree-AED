#pragma once
#include <vector>
#include <memory>
#include "AABB.h"
#include "Metrics.h"

class QuadTree {
public:
    struct Item { int id; Vec2 pos; };

    QuadTree(const AABB& boundary, int capacity, int maxDepth = 16, int depth = 0);

    bool insert(const Item& it);
    void clear();

    void queryRange(const AABB& range, vector<int>& out, Metrics& m) const;
    void queryRadius(Vec2 center, float radius, vector<int>& out, Metrics& m) const;

    void collectBoundaries(vector<AABB>& out) const;

private:
    void subdivide();

    AABB boundary;
    int  capacity;
    int  maxDepth, depth;
    bool divided = false;
    vector<Item> points;
    unique_ptr<QuadTree> nw, ne, sw, se;
};