#pragma once
#include <cmath>
#include <algorithm>
#include "Vec2.h"

struct AABB {
    Vec2 center;
    Vec2 half;

    bool contains(Vec2 p) const {
        return p.x >= center.x - half.x && p.x <= center.x + half.x &&
               p.y >= center.y - half.y && p.y <= center.y + half.y;
    }

    bool intersects(const AABB& o) const {
        return abs(center.x - o.center.x) <= half.x + o.half.x &&
               abs(center.y - o.center.y) <= half.y + o.half.y;
    }

    bool intersectsCircle(Vec2 c, float r) const {
        float dx = max(0.0f, abs(c.x - center.x) - half.x);
        float dy = max(0.0f, abs(c.y - center.y) - half.y);
        return dx * dx + dy * dy <= r * r;
    }
};