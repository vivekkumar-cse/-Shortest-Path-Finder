#include "ui/UiManager.h"
#include "utilities/JsonIO.h"
#include <imgui.h>
#include <cstring>

namespace ronin::ui {

static const char* kAlgoNames[] = {
    "Dijkstra", "A*", "BFS", "DFS", "Greedy Best-First",
    "Bellman-Ford", "Bidirectional BFS", "Floyd-Warshall", "Jump Point Search"
};
static const char* kHeuristicNames[] = {"Manhattan", "Euclidean", "Diagonal", "Chebyshev"};
static const char* kTerrainNames[]  = {"Empty", "Grass", "Sand", "Water", "Lava", "Wall"};
static const char* kThemeNames[]    = {"Samurai Neon", "Matrix", "Tron", "Dark Minimal"};

void UiManager::draw(engine::Simulation& sim, engine::ThemeId& theme,
                     grid::Terrain& brush, bool& showMinimap) {
    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin("Ronin Pathfinder Engine");

    ImGui::TextColored(ImVec4(1,0.85f,0.3f,1), "侍  Ronin Pathfinder");
    ImGui::Separator();

    // Mode toggle
    int mode = (sim.mode == engine::Mode::Grid) ? 0 : 1;
    if (ImGui::RadioButton("Grid", &mode, 0)) sim.mode = engine::Mode::Grid;
    ImGui::SameLine();
    if (ImGui::RadioButton("Graph", &mode, 1)) sim.mode = engine::Mode::Graph;

    ImGui::SeparatorText("Algorithm");
    int algo = static_cast<int>(sim.currentAlgo());
    if (ImGui::Combo("Primary", &algo, kAlgoNames, IM_ARRAYSIZE(kAlgoNames)))
        sim.selectAlgorithm(static_cast<engine::AlgorithmId>(algo));

    ImGui::Checkbox("Comparison mode", &sim.comparisonEnabled);
    if (sim.comparisonEnabled) {
        int c = static_cast<int>(sim.compareAlgo());
        if (ImGui::Combo("Secondary", &c, kAlgoNames, IM_ARRAYSIZE(kAlgoNames)))
            sim.selectComparisonAlgorithm(static_cast<engine::AlgorithmId>(c));
    }

    int hi = static_cast<int>(sim.config().heuristic);
    if (ImGui::Combo("Heuristic", &hi, kHeuristicNames, IM_ARRAYSIZE(kHeuristicNames)))
        sim.config().heuristic = static_cast<algorithms::Heuristic>(hi);
    ImGui::Checkbox("Allow diagonal", &sim.config().allowDiagonal);

    ImGui::SliderFloat("Steps/sec", &sim.stepsPerSecond, 1.f, 1000.f, "%.0f");

    ImGui::SeparatorText("Run");
    if (sim.isRunning()) { if (ImGui::Button("Pause", ImVec2(-1, 0))) sim.pause(); }
    else                 { if (ImGui::Button("Play",  ImVec2(-1, 0))) sim.play();  }
    if (ImGui::Button("Reset traversal (R)", ImVec2(-1, 0))) sim.resetTraversal();
    if (ImGui::Button("Clear grid (C)",      ImVec2(-1, 0))) sim.clearAll();

    ImGui::SeparatorText("Terrain brush");
    int t = static_cast<int>(brush);
    if (ImGui::Combo("Brush", &t, kTerrainNames, IM_ARRAYSIZE(kTerrainNames)))
        brush = static_cast<grid::Terrain>(t);
    ImGui::TextDisabled("S+click: start | E+click: end\nLMB paint | RMB erase");

    ImGui::SeparatorText("Maze (M)");
    if (ImGui::Button("Recursive Backtrack")) sim.startMaze(0);
    ImGui::SameLine();
    if (ImGui::Button("Prim"))                sim.startMaze(1);
    ImGui::SameLine();
    if (ImGui::Button("Kruskal"))             sim.startMaze(2);

    ImGui::SeparatorText("Dynamic / AI");
    if (ImGui::Checkbox("Dynamic obstacles", &sim.dynamicObstaclesEnabled)) {
        if (sim.dynamicObstaclesEnabled) sim.dynObs().spawn(sim.grid(), 12);
        else                              sim.dynObs().clear(sim.grid());
    }
    ImGui::Checkbox("AI agent", &sim.agentEnabled);
    if (sim.agentEnabled && sim.agent().position() < 0)
        sim.agent().setPosition(sim.grid().start());

    ImGui::SeparatorText("Theme & View");
    int th = static_cast<int>(theme);
    if (ImGui::Combo("Theme", &th, kThemeNames, IM_ARRAYSIZE(kThemeNames))) {
        theme = static_cast<engine::ThemeId>(th);
        engine::applyImGuiTheme(theme);
    }
    ImGui::Checkbox("Minimap", &showMinimap);

    ImGui::SeparatorText("Save / Load (JSON)");
    static char path[256] = "maps/last.json";
    ImGui::InputText("Path", path, sizeof(path));
    if (ImGui::Button("Save")) utilities::saveMap(path, sim.grid());
    ImGui::SameLine();
    if (ImGui::Button("Load")) { utilities::loadMap(path, sim.grid()); sim.resetTraversal(); }

    ImGui::End();

    // ============ Stats panel ============
    ImGui::SetNextWindowPos(ImVec2(12, 740), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(340, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Statistics");
    const auto& s = sim.stats();
    ImGui::Text("Algorithm   : %s", s.algorithmName);
    ImGui::Text("Complexity  : %s", s.complexity);
    ImGui::Text("Visited     : %d", s.nodesVisited);
    ImGui::Text("Path length : %d", s.pathLength);
    ImGui::Text("Exec time   : %.2f ms", s.execTimeMs);
    ImGui::Text("FPS         : %.0f", s.fps);
    if (sim.comparisonEnabled) {
        ImGui::Separator();
        const auto& c = sim.compareStats();
        ImGui::TextColored(ImVec4(1, 0.6f, 0.6f, 1), "Compare: %s", c.algorithmName);
        ImGui::Text("Visited     : %d", c.nodesVisited);
        ImGui::Text("Path length : %d", c.pathLength);
        ImGui::Text("Exec time   : %.2f ms", c.execTimeMs);
    }
    ImGui::End();
}

} // namespace ronin::ui
