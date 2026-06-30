#include "Renderer.h"
#include "QuadTree.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>

static constexpr Color C_BG = {12, 12, 22, 255};
static constexpr Color C_VP_BG = {8, 8, 18, 255};
static constexpr Color C_BORDER = {55, 55, 90, 255};
static constexpr Color C_BOUNDS = {0, 210, 90, 50};
static constexpr Color C_PARTICLE = {100, 180, 255, 210}; 
static constexpr Color C_CANDIDATE = {255, 220, 0, 255};  
static constexpr Color C_COLLIDER = {255, 55, 55, 255};   
static constexpr Color C_QUERY_FILL = {0, 200, 255, 30};  
static constexpr Color C_QUERY_LINE = {0, 200, 255, 200}; 
static constexpr Color C_CIRC_FILL = {180, 0, 255, 30};   
static constexpr Color C_CIRC_LINE = {180, 0, 255, 200};  
static constexpr Color C_HUD_BG = {16, 16, 28, 255};
static constexpr Color C_HUD_SEP = {38, 38, 65, 255};
static constexpr Color C_TXT = {218, 218, 232, 255};
static constexpr Color C_TXT_DIM = {110, 110, 140, 255};
static constexpr Color C_TXT_GREEN = {70, 220, 95, 255};
static constexpr Color C_TXT_YELLOW = {255, 215, 0, 255};
static constexpr Color C_TXT_RED = {255, 75, 75, 255};
static constexpr Color C_TXT_BLUE = {90, 185, 255, 255};
static constexpr Color C_TXT_PURPLE = {190, 90, 255, 255};

static std::string abbrev(long long v)
{
    if (v >= 1'000'000)
    {
        std::ostringstream s;
        s << std::fixed << std::setprecision(1) << v / 1'000'000.0 << "M";
        return s.str();
    }
    if (v >= 1'000)
    {
        std::ostringstream s;
        s << std::fixed << std::setprecision(1) << v / 1'000.0 << "K";
        return s.str();
    }
    return std::to_string(v);
}

static std::string fmtMs(double ms)
{
    std::ostringstream s;
    s << std::fixed << std::setprecision(2) << ms << " ms";
    return s.str();
}

Renderer::Renderer(int wW, int wH, float wldW, float wldH)
    : winW(wW), winH(wH), worldW(wldW), worldH(wldH)
{

    const int margin = 10;
    vpH = winH - 2 * margin;
    vpW = vpH;
    vpX = margin;
    vpY = margin;

    hudX = vpX + vpW + margin;
    hudW = winW - hudX - margin;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(winW, winH, "Simulador de partículas con QuadTree | [R] Rectángulo [C] Círculo [SPACE] Pausar [Q] Bordes [H] Ayuda");
    SetTargetFPS(60);
}

Renderer::~Renderer()
{
    CloseWindow();
}

void Renderer::setConfig(const std::string &dist, int n, int capacity, unsigned seed)
{
    cfgDist = dist;
    cfgN    = n;
    cfgCap  = capacity;
    cfgSeed = seed;
}

Vector2 Renderer::toScreen(float wx, float wy) const
{
    return {vpX + (wx / worldW) * vpW,
            vpY + (wy / worldH) * vpH};
}

float Renderer::toScreenLen(float wLen) const
{
    return (wLen / worldW) * vpW;
}

Vec2 Renderer::toWorld(float sx, float sy) const
{
    return {((sx - vpX) / vpW) * worldW,
            ((sy - vpY) / vpH) * worldH};
}

bool Renderer::inViewport(Vector2 p) const
{
    return p.x >= vpX && p.x <= vpX + vpW &&
           p.y >= vpY && p.y <= vpY + vpH;
}

void Renderer::handleInput(const Simulation &sim)
{
    if (IsKeyPressed(KEY_Q))
        showBounds = !showBounds;
    if (IsKeyPressed(KEY_H))
        showHelp = !showHelp;

    if (IsKeyPressed(KEY_R))
    {
        qMode = (qMode == QueryMode::Rect) ? QueryMode::None : QueryMode::Rect;
        qState = QueryState::Idle;
        candidateSet.clear();
        manualQueryMetrics.reset();
    }
    if (IsKeyPressed(KEY_C))
    {
        qMode = (qMode == QueryMode::Circle) ? QueryMode::None : QueryMode::Circle;
        qState = QueryState::Idle;
        candidateSet.clear();
        manualQueryMetrics.reset();
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        qMode = QueryMode::None;
        qState = QueryState::Idle;
        candidateSet.clear();
        manualQueryMetrics.reset();
    }

    if (qMode == QueryMode::Rect)
        updateRectQuery(sim);
    if (qMode == QueryMode::Circle)
        updateCircleQuery(sim);
}

void Renderer::updateRectQuery(const Simulation &sim)
{
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && inViewport(mouse))
    {
        dragStart = mouse;
        dragCurrent = mouse;
        qState = QueryState::Dragging;
        candidateSet.clear();
        manualQueryMetrics.reset();
    }

    if (qState == QueryState::Dragging)
    {
        dragCurrent = mouse;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            qState = QueryState::Done;

            float x0 = std::min(dragStart.x, dragCurrent.x);
            float y0 = std::min(dragStart.y, dragCurrent.y);
            float x1 = std::max(dragStart.x, dragCurrent.x);
            float y1 = std::max(dragStart.y, dragCurrent.y);

            Vec2 wTL = toWorld(x0, y0);
            Vec2 wBR = toWorld(x1, y1);
            Vec2 wCenter = {(wTL.x + wBR.x) * 0.5f, (wTL.y + wBR.y) * 0.5f};
            Vec2 wHalf = {(wBR.x - wTL.x) * 0.5f, (wBR.y - wTL.y) * 0.5f};

            std::vector<int> out;
            sim.tree().queryRange(AABB{wCenter, wHalf}, out, manualQueryMetrics);
            candidateSet.clear();
            for (int id : out)
                candidateSet.insert(id);
        }
    }
}

void Renderer::updateCircleQuery(const Simulation &sim)
{
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && inViewport(mouse))
    {
        dragStart = mouse;
        dragCurrent = mouse;
        qState = QueryState::Dragging;
        candidateSet.clear();
        manualQueryMetrics.reset();
    }

    if (qState == QueryState::Dragging)
    {
        dragCurrent = mouse;

        float dx = dragCurrent.x - dragStart.x;
        float dy = dragCurrent.y - dragStart.y;
        float radiusPx = std::sqrt(dx * dx + dy * dy);

        if (radiusPx > 3.f)
        {
            Vec2 wCenter = toWorld(dragStart.x, dragStart.y);
            float wRadius = (radiusPx / vpW) * worldW;

            std::vector<int> out;
            manualQueryMetrics.reset();
            sim.tree().queryRadius(wCenter, wRadius, out, manualQueryMetrics);
            candidateSet.clear();
            for (int id : out)
                candidateSet.insert(id);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            qState = QueryState::Done;
    }
}

void Renderer::draw(const Simulation &sim,
                    const std::vector<std::pair<int, int>> &liveCollisions,
                    const Metrics &qtM,
                    const Metrics &bfM,
                    bool simPaused)
{

    std::unordered_set<int> colliderSet;
    colliderSet.reserve(liveCollisions.size() * 2);
    for (const auto &[a, b] : liveCollisions)
    {
        colliderSet.insert(a);
        colliderSet.insert(b);
    }

    BeginDrawing();
    ClearBackground(C_BG);

    DrawRectangle(vpX, vpY, vpW, vpH, C_VP_BG);

    BeginScissorMode(vpX, vpY, vpW, vpH);

    if (showBounds)
        drawQuadTreeBounds(sim.tree());

    drawQueryRegion();

    drawParticles(sim.particles(), candidateSet, colliderSet);

    EndScissorMode();

    drawViewportBorder();

    drawHUD(sim, qtM, bfM, (int)liveCollisions.size(), simPaused);

    EndDrawing();
}

void Renderer::drawViewportBorder()
{
    DrawRectangleLinesEx({(float)vpX, (float)vpY, (float)vpW, (float)vpH},
                         2.f, C_BORDER);
}

void Renderer::drawQuadTreeBounds(const QuadTree &qt)
{
    std::vector<AABB> cells;
    cells.reserve(512);
    qt.collectBoundaries(cells);

    for (const auto &cell : cells)
    {
        float sx = vpX + ((cell.center.x - cell.half.x) / worldW) * vpW;
        float sy = vpY + ((cell.center.y - cell.half.y) / worldH) * vpH;
        float sw = (cell.half.x * 2.f / worldW) * vpW;
        float sh = (cell.half.y * 2.f / worldH) * vpH;
        DrawRectangleLinesEx({sx, sy, sw, sh}, 1.f, C_BOUNDS);
    }
}

void Renderer::drawQueryRegion()
{
    if (qMode == QueryMode::Rect &&
        (qState == QueryState::Dragging || qState == QueryState::Done))
    {

        float x0 = std::min(dragStart.x, dragCurrent.x);
        float y0 = std::min(dragStart.y, dragCurrent.y);
        float w = std::abs(dragCurrent.x - dragStart.x);
        float h = std::abs(dragCurrent.y - dragStart.y);
        DrawRectangle((int)x0, (int)y0, (int)w, (int)h, C_QUERY_FILL);
        DrawRectangleLinesEx({x0, y0, w, h}, 2.f, C_QUERY_LINE);
    }

    if (qMode == QueryMode::Circle &&
        (qState == QueryState::Dragging || qState == QueryState::Done))
    {

        float dx = dragCurrent.x - dragStart.x;
        float dy = dragCurrent.y - dragStart.y;
        float r = std::sqrt(dx * dx + dy * dy);
        DrawCircle((int)dragStart.x, (int)dragStart.y, r, C_CIRC_FILL);
        DrawCircleLines((int)dragStart.x, (int)dragStart.y, r, C_CIRC_LINE);
        DrawLine((int)dragStart.x - 6, (int)dragStart.y,
                 (int)dragStart.x + 6, (int)dragStart.y, C_CIRC_LINE);
        DrawLine((int)dragStart.x, (int)dragStart.y - 6,
                 (int)dragStart.x, (int)dragStart.y + 6, C_CIRC_LINE);
    }
}

void Renderer::drawParticles(const std::vector<Particle> &parts,
                             const std::unordered_set<int> &candidates,
                             const std::unordered_set<int> &colliders)
{
    for (const auto &p : parts)
    {
        Vector2 sc = toScreen((float)p.x, (float)p.y);
        float sr = std::max(1.5f, toScreenLen((float)p.radius));

        Color fill = C_PARTICLE;
        if (colliders.count(p.id))
            fill = C_COLLIDER;
        else if (candidates.count(p.id))
            fill = C_CANDIDATE;

        DrawCircleV(sc, sr, fill);

        Color outline = {(unsigned char)(fill.r / 3),
                         (unsigned char)(fill.g / 3),
                         (unsigned char)(fill.b / 3), 160};
        DrawCircleLines((int)sc.x, (int)sc.y, sr, outline);
    }
}

void Renderer::drawHUD(const Simulation &sim,
                       const Metrics &qtM,
                       const Metrics &bfM,
                       int collisionCount,
                       bool paused)
{

    DrawRectangle(hudX - 4, 0, hudW + 8, winH, C_HUD_BG);

    const int x = hudX + 4;
    const int fsSec = 14; // tamaño del encabezado de sección
    const int fsTxt = 14; // tamaño del texto general
    const int lineH = 20;
    const int secGap = 10;
    int y = 12;

    auto sep = [&]()
    {
        DrawLine(x, y, x + hudW - 8, y, C_HUD_SEP);
        y += secGap;
    };

    auto kv = [&](const char *label, const char *value, Color valCol)
    {
        DrawText(label, x, y, fsTxt, C_TXT_DIM);
        int lw = MeasureText(label, fsTxt);
        DrawText(value, x + lw + 4, y, fsTxt, valCol);
        y += lineH;
    };

    auto section = [&](const char *title)
    {
        DrawText(title, x, y, fsSec, C_TXT_DIM);
        y += lineH;
    };

    DrawText("Visualizador de QuadTree", x, y, 16, C_TXT);
    y += 22;
    if (paused)
    {
        DrawText("[ PAUSADO ]", x + 2, y, fsTxt, C_TXT_YELLOW);
        y += lineH;
    }
    sep();

    section("Configuracion");
    char cbuf[64];
    kv("Distribucion:", cfgDist.c_str(), C_TXT_PURPLE);
    snprintf(cbuf, sizeof(cbuf), "%d", cfgN);
    kv("Particulas (n):", cbuf, C_TXT);
    snprintf(cbuf, sizeof(cbuf), "%d", cfgCap);
    kv("Capacidad/nodo:", cbuf, C_TXT);
    snprintf(cbuf, sizeof(cbuf), "%u", cfgSeed);
    kv("Semilla:", cbuf, C_TXT_DIM);
    y += secGap;
    sep();

    section("Rendimiento");
    int fps = GetFPS();
    char buf[64];
    snprintf(buf, sizeof(buf), "%d", fps);
    kv("FPS:     ", buf, fps >= 55 ? C_TXT_GREEN : (fps >= 30 ? C_TXT_YELLOW : C_TXT_RED));

    snprintf(buf, sizeof(buf), "%.2f ms", GetFrameTime() * 1000.f);
    kv("Fotograma:", buf, C_TXT);
    y += secGap;
    sep();

    section("Simulación");
    snprintf(buf, sizeof(buf), "%d", (int)sim.particles().size());
    kv("Partículas:", buf, C_TXT);

    snprintf(buf, sizeof(buf), "%d pares", collisionCount);
    kv("Colisiones:", buf, collisionCount > 0 ? C_TXT_RED : C_TXT);
    y += secGap;
    sep();


    section("Comparaciones en vivo (por fotograma)");

    DrawText("QuadTree", x, y, fsTxt, C_TXT_BLUE);
    y += lineH - 2;
    kv("  Comparaciones:", abbrev(qtM.comparisons).c_str(), C_TXT_GREEN);
    kv("  Nodos visitados:", abbrev(qtM.nodesVisited).c_str(), C_TXT);
    kv("  Candidatos:", abbrev(qtM.candidates).c_str(), C_TXT);
    y += 4;

    DrawText("Fuerza bruta", x, y, fsTxt, {255, 140, 60, 255});
    y += lineH - 2;
    if (bfM.comparisons > 0)
    {
        kv("  Comparaciones:", abbrev(bfM.comparisons).c_str(), C_TXT_RED);
    }
    else
    {
        kv("  Comparaciones:", "N > 3000 (est.)", C_TXT_DIM);
        long long n = (long long)sim.particles().size();
        kv("  Teórico:", abbrev(n * (n - 1) / 2).c_str(), {180, 80, 80, 255});
    }
    y += 6;

    if (bfM.comparisons > 0 && qtM.comparisons > 0)
    {
        float ratio = (float)bfM.comparisons / (float)(qtM.comparisons > 0 ? qtM.comparisons : 1);
        snprintf(buf, sizeof(buf), "%.1fx más rápido", ratio);
        DrawText("Aceleración QT:", x, y, fsTxt, C_TXT_DIM);
        int lw = MeasureText("Aceleración QT:", fsTxt);
        DrawText(buf, x + lw + 4, y, fsTxt, C_TXT_YELLOW);
        y += lineH;
    }
    y += secGap;
    sep();

    if (qMode != QueryMode::None)
    {
        section("Consulta manual");

        const char *modeStr = (qMode == QueryMode::Rect) ? "Rectángulo (AABB)" : "Círculo (radio)";
        kv("Modo:    ", modeStr, (qMode == QueryMode::Rect) ? C_QUERY_LINE : C_CIRC_LINE);

        snprintf(buf, sizeof(buf), "%d", (int)candidateSet.size());
        kv("Candidatos:", buf, C_TXT_YELLOW);

        if (manualQueryMetrics.comparisons > 0)
        {
            kv("Comparaciones:", abbrev(manualQueryMetrics.comparisons).c_str(), C_TXT);
            kv("Nodos visitados:", abbrev(manualQueryMetrics.nodesVisited).c_str(), C_TXT_DIM);
        }

        const char *stateStr = "";
        if (qState == QueryState::Idle)
            stateStr = "Dibuja el área.";
        if (qState == QueryState::Dragging)
            stateStr = "Arrastrando...";
        if (qState == QueryState::Done)
            stateStr = "Listo. Arrastra otra vez para repetir.";
        DrawText(stateStr, x, y, fsTxt - 1, C_TXT_DIM);
        y += lineH;
        y += secGap;
        sep();
    }

    section("Leyenda");
    auto legend = [&](Color dot, const char *label)
    {
        DrawCircle(x + 7, y + 7, 6, dot);
        DrawText(label, x + 18, y, fsTxt, C_TXT);
        y += lineH;
    };
    legend(C_PARTICLE, "Partícula normal");
    legend(C_CANDIDATE, "Dentro de la región de consulta");
    legend(C_COLLIDER, "Colisión detectada");

    DrawRectangle(x + 1, y + 3, 13, 10, C_BOUNDS);
    DrawRectangleLinesEx({(float)x + 1, (float)y + 3, 13, 10}, 1.f, C_TXT_DIM);
    DrawText("Celda de QuadTree", x + 18, y, fsTxt, C_TXT);
    y += lineH;
    y += secGap;
    sep();

    if (showHelp)
    {
        section("Controles");
        auto key = [&](const char *k, const char *desc)
        {
            DrawText(k, x, y, fsTxt, C_TXT_YELLOW);
            int kw = MeasureText(k, fsTxt);
            DrawText(desc, x + kw + 4, y, fsTxt, C_TXT_DIM);
            y += lineH;
        };
        key("[R]", " Consulta con rectangulo");
        key("[C]", " Consulta con circulo");
        key("[Q]", " Alternar bordes de QT");
        key("[1/2/3]", " Distribucion Unif/Clust/Densa");
        key("[G]", " Nueva semilla (regenerar)");
        key("[+/-]", " Mas / menos particulas");
        key("[ [ / ] ]", " Capacidad de nodo  -/+");
        key("[SPACE]", " Pausar / Reanudar");
        key("[H]", " Ocultar los controles");
        key("[ESC]", " Salir del programa");
    }
    else
    {
        DrawText("[H] Mostrar controles", x, y, fsTxt, C_TXT_DIM);
        y += lineH;
    }
}
