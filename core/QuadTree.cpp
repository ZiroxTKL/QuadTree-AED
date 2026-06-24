#include "QuadTree.h"

QuadTree::QuadTree(const AABB& b, int cap, int maxD, int d)
    : boundary(b), capacity(cap), maxDepth(maxD), depth(d) {}

bool QuadTree::insert(const Item& it) {
    if (!boundary.contains(it.pos)) return false;

    if (divided)
        return nw->insert(it) || ne->insert(it) ||
               sw->insert(it) || se->insert(it);

    points.push_back(it);
    if ((int)points.size() > capacity && depth < maxDepth)
        subdivide();
    return true;
}

void QuadTree::subdivide() {
    Vec2 h = { boundary.half.x * 0.5f, boundary.half.y * 0.5f };
    Vec2 c = boundary.center;
    nw = std::make_unique<QuadTree>(AABB{ {c.x - h.x, c.y - h.y}, h }, capacity, maxDepth, depth + 1);
    ne = std::make_unique<QuadTree>(AABB{ {c.x + h.x, c.y - h.y}, h }, capacity, maxDepth, depth + 1);
    sw = std::make_unique<QuadTree>(AABB{ {c.x - h.x, c.y + h.y}, h }, capacity, maxDepth, depth + 1);
    se = std::make_unique<QuadTree>(AABB{ {c.x + h.x, c.y + h.y}, h }, capacity, maxDepth, depth + 1);
    divided = true;

    for (const auto& it : points)
        nw->insert(it) || ne->insert(it) || sw->insert(it) || se->insert(it);
    points.clear();
}

void QuadTree::clear() {
    points.clear();
    divided = false;
    nw = nullptr;
    ne = nullptr;
    sw = nullptr;
    se = nullptr;
}

void QuadTree::queryRange(const AABB& range, std::vector<int>& out, Metrics& m) const {
    m.nodesVisited++;
    if (!boundary.intersects(range)) return;

    for (const auto& it : points) {
        m.comparisons++;
        if (range.contains(it.pos)) out.push_back(it.id);
    }
    if (divided) {
        nw->queryRange(range, out, m);
        ne->queryRange(range, out, m);
        sw->queryRange(range, out, m);
        se->queryRange(range, out, m);
    }
}

void QuadTree::queryRadius(Vec2 center, float radius, std::vector<int>& out, Metrics& m) const {
    m.nodesVisited++;
    if (!boundary.intersectsCircle(center, radius)) return;

    float r2 = radius * radius;
    for (const auto& it : points) {
        m.comparisons++;
        if ((it.pos - center).len2() <= r2) out.push_back(it.id);
    }
    if (divided) {
        nw->queryRadius(center, radius, out, m);
        ne->queryRadius(center, radius, out, m);
        sw->queryRadius(center, radius, out, m);
        se->queryRadius(center, radius, out, m);
    }
}

void QuadTree::collectBoundaries(std::vector<AABB>& out) const {
    out.push_back(boundary);
    if (divided) {
        nw->collectBoundaries(out);
        ne->collectBoundaries(out);
        sw->collectBoundaries(out);
        se->collectBoundaries(out);
    }
}