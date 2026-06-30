#pragma once
#include <vector>
#include <utility>
#include <unordered_set>
#include <string>
#include "raylib.h"
#include "Simulation.h"
#include "Metrics.h"
#include "AABB.h"
#include "Vec2.h"

enum class QueryMode  { None, Rect, Circle };
enum class QueryState { Idle, Dragging, Done };

class Renderer {
public:
    Renderer(int windowW, int windowH, float worldW, float worldH);
    ~Renderer();

    void handleInput(const Simulation& sim);
    void resetQuery();
    void setWorldSize(float worldW, float worldH);
    void setScenarioInfo(const std::string& distribution,
                         int capacity,
                         float minRadius,
                         float maxRadius,
                         float maxSpeed);

    void draw(const Simulation&              sim,
              const std::vector<std::pair<int,int>>& liveCollisions,
              const Metrics&                 qtLiveMetrics,
              const Metrics&                 bfLiveMetrics,
              bool                           simPaused);

    bool windowShouldClose() const { return WindowShouldClose(); }

private:
    Vector2 toScreen(float wx, float wy)   const;
    float   toScreenLen(float worldLen)    const;
    Vec2    toWorld(float sx, float sy)    const;
    bool    inViewport(Vector2 screenPt)   const;

    void drawViewportBorder();
    void drawQuadTreeBounds(const QuadTree& qt);
    void drawQueryRegion();
    void drawParticles(const std::vector<Particle>&  parts,
                       const std::unordered_set<int>& candidates,
                       const std::unordered_set<int>& colliders);
    void drawHUD(const Simulation& sim,
                 const Metrics&    qtM,
                 const Metrics&    bfM,
                 int               collisionCount,
                 bool              paused);

    void updateRectQuery(const Simulation& sim);
    void updateCircleQuery(const Simulation& sim);

    int   winW, winH;
    float worldW, worldH;

    int vpX, vpY, vpW, vpH;
    int hudX, hudW;

    bool showBounds = true;
    bool showHelp   = true;

    QueryMode  qMode  = QueryMode::None;
    QueryState qState = QueryState::Idle;

    Vector2 dragStart{0, 0};
    Vector2 dragCurrent{0, 0};

    std::unordered_set<int> candidateSet;
    Metrics manualQueryMetrics;

    std::string distributionName = "Uniforme";
    int scenarioCapacity = 8;
    float scenarioMinRadius = 3.f;
    float scenarioMaxRadius = 6.f;
    float scenarioMaxSpeed = 80.f;
};
