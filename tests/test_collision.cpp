#include "test_framework.hpp"
#include "core/Config.hpp"
#include "quadtree/QuadTree.hpp"
#include "simulation/Collision.hpp"
#include "simulation/ParticleGenerator.hpp"
#include "benchmark/BruteForce.hpp"

#include <set>
#include <utility>

using namespace qt;

// Normaliza un conjunto de pares a (min,max) para comparar sin importar orden.
static std::set<std::pair<int,int>> toSet(const std::vector<CollisionPair>& pairs) {
    std::set<std::pair<int,int>> s;
    for (const auto& p : pairs)
        s.insert({std::min(p.a, p.b), std::max(p.a, p.b)});
    return s;
}

// Corre el caso para una distribución y comprueba que QuadTree == fuerza bruta.
static void runCase(Distribution dist, int n, std::set<std::pair<int,int>>& outQt) {
    Config cfg;
    cfg.n = n;
    cfg.width = cfg.height = 500.0;
    cfg.distribution = dist;
    cfg.radiusMin = cfg.radiusMax = 4.0;  // radio fijo para un caso claro
    cfg.seed = 123;

    ParticleGenerator gen(cfg);
    auto ps = gen.generate();

    Rectangle space(0, 0, cfg.width, cfg.height);
    QuadTree tree(space, cfg.capacity, cfg.maxDepth);
    tree.build(ps);

    CollisionResult qt = Collision::detect(ps, tree);
    CollisionResult bf = BruteForce::detectCollisions(ps);

    auto qtSet = toSet(qt.pairs);
    auto bfSet = toSet(bf.pairs);

    CHECK(qtSet == bfSet);
    // La fuerza bruta hace exactamente n(n-1)/2 comparaciones.
    CHECK(bf.comparisons == static_cast<int64_t>(n) * (n - 1) / 2);

    outQt = qtSet;
}

int main() {
    std::cout << "== test_collision ==\n";

    std::set<std::pair<int,int>> tmp;

    SECTION("uniforme: QuadTree == fuerza bruta");
    runCase(Distribution::Uniform, 800, tmp);

    SECTION("clusters: QuadTree == fuerza bruta");
    runCase(Distribution::Clusters, 800, tmp);

    SECTION("zona densa: QuadTree == fuerza bruta");
    runCase(Distribution::Dense, 800, tmp);

    SECTION("dos particulas que se solapan se detectan");
    {
        std::vector<Particle> ps;
        ps.emplace_back(0, 100.0, 100.0, 0, 0, 5.0);
        ps.emplace_back(1, 104.0, 100.0, 0, 0, 5.0);  // distancia 4 < 10 => colisión
        ps.emplace_back(2, 300.0, 300.0, 0, 0, 5.0);  // aislada

        Rectangle space(0, 0, 500, 500);
        QuadTree tree(space, 4);
        tree.build(ps);

        CollisionResult qt = Collision::detect(ps, tree);
        auto s = toSet(qt.pairs);
        CHECK(s.size() == 1);
        CHECK(s.count({0, 1}) == 1);
    }

    SECTION("dos particulas separadas no colisionan");
    {
        std::vector<Particle> ps;
        ps.emplace_back(0, 100.0, 100.0, 0, 0, 5.0);
        ps.emplace_back(1, 130.0, 100.0, 0, 0, 5.0);  // distancia 30 > 10

        Rectangle space(0, 0, 500, 500);
        QuadTree tree(space, 4);
        tree.build(ps);

        CollisionResult qt = Collision::detect(ps, tree);
        CHECK(qt.pairs.empty());
    }

    return qt_test::summary("test_collision");
}
