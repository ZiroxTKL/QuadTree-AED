#pragma once

struct Metrics {

    long long comparisons  = 0;
    long long nodesVisited = 0;
    long long candidates   = 0;
    double    buildTimeMs  = 0;
    double    queryTimeMs  = 0;
    void reset() {
         *this = Metrics{};
    }

};