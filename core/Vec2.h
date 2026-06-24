#pragma once

using namespace std;

struct Vec2 {

    float x = 0, y = 0;
    Vec2 operator+(Vec2 o) const { 
        return {x + o.x, y + o.y};
    }

    Vec2 operator-(Vec2 o) const {
         return {x - o.x, y - o.y}; 
    }

    Vec2 operator*(float s) const {
         return {x * s, y * s};
    }

    float len2() const {
        return x * x + y * y;
    }

};