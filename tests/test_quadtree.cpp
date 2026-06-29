#include "test_framework.hpp"
#include "quadtree/QuadTree.hpp"
#include "benchmark/BruteForce.hpp"

#include <vector>
#include <algorithm>
#include <set>

using namespace qt;

// Construye partículas en una rejilla determinista para tests reproducibles.
static std::vector<Particle> gridParticles(int side, double spacing, double radius) {
    std::vector<Particle> ps;
    int id = 0;
    for (int i = 0; i < side; ++i)
        for (int j = 0; j < side; ++j)
            ps.emplace_back(id++, (i + 0.5) * spacing, (j + 0.5) * spacing, 0.0, 0.0, radius);
    return ps;
}

int main() {
    std::cout << "== test_quadtree ==\n";

    SECTION("construccion basica e inserta todo");
    {
        auto ps = gridParticles(10, 10.0, 1.0);   // 100 partículas en 100x100
        Rectangle space(0, 0, 100, 100);
        QuadTree tree(space, 4);
        tree.build(ps);

        // Una query que cubre todo el espacio debe devolver las 100 partículas.
        int64_t cmp = 0;
        auto all = tree.queryRange(space, cmp);
        CHECK(all.size() == ps.size());
        CHECK(tree.nodeCount() > 1);  // con 100 partículas y cap=4, hay subdivisión
    }

    SECTION("queryRange coincide con fuerza bruta");
    {
        auto ps = gridParticles(12, 8.0, 1.0);
        Rectangle space(0, 0, 96, 96);
        QuadTree tree(space, 4);
        tree.build(ps);

        Rectangle region(20, 20, 30, 30);

        int64_t cmp = 0;
        auto got = tree.queryRange(region, cmp);

        // Referencia por fuerza bruta.
        std::set<int> expected;
        for (const auto& p : ps)
            if (region.contains(p)) expected.insert(p.id);

        std::set<int> gotIds;
        for (auto* p : got) gotIds.insert(p->id);

        CHECK(gotIds == expected);
    }

    SECTION("queryNeighbors coincide con fuerza bruta");
    {
        auto ps = gridParticles(15, 6.0, 1.0);
        Rectangle space(0, 0, 90, 90);
        QuadTree tree(space, 4);
        tree.build(ps);

        double qx = 45, qy = 45, r = 15;

        int64_t cmp = 0;
        auto got = tree.queryNeighbors(qx, qy, r, cmp);

        int64_t bfCmp = 0;
        auto ref = BruteForce::queryNeighbors(ps, qx, qy, r, bfCmp);

        std::set<int> gotIds, refIds;
        for (auto* p : got) gotIds.insert(p->id);
        for (auto* p : ref) refIds.insert(p->id);

        CHECK(gotIds == refIds);
        // El QuadTree debe hacer menos comparaciones que revisar todo n.
        CHECK(cmp < static_cast<int64_t>(ps.size()) * 4);
    }

    SECTION("capacidad respetada provoca subdivision");
    {
        // 5 partículas casi en el mismo punto con cap=4 fuerzan subdivisión.
        std::vector<Particle> ps;
        for (int i = 0; i < 5; ++i)
            ps.emplace_back(i, 10.0 + i * 0.01, 10.0 + i * 0.01, 0, 0, 0.5);

        Rectangle space(0, 0, 100, 100);
        QuadTree tree(space, 4);
        tree.build(ps);

        CHECK(tree.nodeCount() >= 5);  // raíz + 4 hijos como mínimo
    }

    SECTION("clear deja el arbol vacio");
    {
        auto ps = gridParticles(8, 10.0, 1.0);
        Rectangle space(0, 0, 80, 80);
        QuadTree tree(space, 4);
        tree.build(ps);
        tree.clear();

        int64_t cmp = 0;
        auto all = tree.queryRange(space, cmp);
        CHECK(all.empty());
        CHECK(tree.nodeCount() == 1);
    }

    return qt_test::summary("test_quadtree");
}
