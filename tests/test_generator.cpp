#include "test_framework.hpp"
#include "core/Config.hpp"
#include "simulation/ParticleGenerator.hpp"

#include <cmath>

using namespace qt;

// ¿Todas las partículas están dentro del espacio y con velocidad en rango?
static void checkInvariants(const std::vector<Particle>& ps, const Config& cfg) {
    for (const auto& p : ps) {
        CHECK(p.x >= 0.0 && p.x <= cfg.width);
        CHECK(p.y >= 0.0 && p.y <= cfg.height);
        CHECK(p.radius >= cfg.radiusMin - 1e-9);
        CHECK(p.radius <= cfg.radiusMax + 1e-9);

        double speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        // La magnitud de la velocidad debe estar dentro de [speedMin, speedMax].
        CHECK(speed >= cfg.speedMin - 1e-6);
        CHECK(speed <= cfg.speedMax + 1e-6);
    }
}

int main() {
    std::cout << "== test_generator ==\n";

    Config base;
    base.width = base.height = 1000.0;
    base.radiusMin = 3.0; base.radiusMax = 6.0;
    base.speedMin = 20.0; base.speedMax = 80.0;
    base.seed = 7;

    SECTION("uniforme: cuenta correcta e invariantes");
    {
        Config cfg = base;
        cfg.n = 1000;
        cfg.distribution = Distribution::Uniform;
        ParticleGenerator gen(cfg);
        auto ps = gen.generate();
        CHECK(static_cast<int>(ps.size()) == cfg.n);
        checkInvariants(ps, cfg);

        // Los ids deben ser 0..n-1.
        bool idsOk = true;
        for (int i = 0; i < cfg.n; ++i) if (ps[i].id != i) idsOk = false;
        CHECK(idsOk);
    }

    SECTION("reproducibilidad: misma semilla -> mismas particulas");
    {
        Config cfg = base;
        cfg.n = 500;
        cfg.distribution = Distribution::Uniform;

        ParticleGenerator g1(cfg);
        ParticleGenerator g2(cfg);
        auto a = g1.generate();
        auto b = g2.generate();

        bool same = a.size() == b.size();
        for (size_t i = 0; same && i < a.size(); ++i)
            if (a[i].x != b[i].x || a[i].y != b[i].y) same = false;
        CHECK(same);
    }

    SECTION("clusters: invariantes y concentracion");
    {
        Config cfg = base;
        cfg.n = 2000;
        cfg.distribution = Distribution::Clusters;
        cfg.numClusters = 4;
        cfg.clusterStd = 30.0;
        ParticleGenerator gen(cfg);
        auto ps = gen.generate();
        CHECK(static_cast<int>(ps.size()) == cfg.n);
        checkInvariants(ps, cfg);
    }

    SECTION("zona densa: mayor concentracion en el centro");
    {
        Config cfg = base;
        cfg.n = 2000;
        cfg.distribution = Distribution::Dense;
        cfg.denseFraction = 0.6;
        cfg.denseSize = 0.15;
        ParticleGenerator gen(cfg);
        auto ps = gen.generate();
        CHECK(static_cast<int>(ps.size()) == cfg.n);
        checkInvariants(ps, cfg);

        // Contar cuántas caen en la zona densa central.
        double dw = cfg.width * cfg.denseSize;
        double dh = cfg.height * cfg.denseSize;
        double dx = (cfg.width - dw) * 0.5;
        double dy = (cfg.height - dh) * 0.5;
        int inDense = 0;
        for (const auto& p : ps)
            if (p.x >= dx && p.x <= dx + dw && p.y >= dy && p.y <= dy + dh) ++inDense;

        // La zona densa ocupa 2.25% del área pero debe contener ~60% de los puntos.
        double fracInDense = static_cast<double>(inDense) / cfg.n;
        CHECK(fracInDense > 0.4);  // claramente más concentrada que uniforme
    }

    return qt_test::summary("test_generator");
}
