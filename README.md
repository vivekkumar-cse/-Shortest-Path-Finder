# Shortest Pathfinder Engine and Graph simulation engine

A futuristic, real-time **shortest-path and graph simulation engine** written in modern
C++17 with **SFML**, **Dear ImGui (via ImGui-SFML)**, and **nlohmann/json**.

> Visualize Dijkstra, A\*, BFS, DFS, Greedy Best-First, Bellman-Ford, Bidirectional BFS,
> Floyd-Warshall, and Jump Point Search on weighted terrain grids and arbitrary graphs,
> with animated maze generation, dynamic obstacles, replay, comparison mode, an AI agent
> simulator, and a neon cyberpunk UI.

---

## Features

- **Grid engine** with terrain (Grass/Sand/Water/Lava/Wall) and per-cell weights
- **Graph mode** with manually-placed weighted nodes and edges
- **9 pathfinding algorithms**, all step-by-step animated, pause/resume capable
- **4 A\* heuristics** (Manhattan, Euclidean, Diagonal, Chebyshev), switchable live
- **Maze generation**: Recursive Backtracking, Prim's, Kruskal's — animated
- **Dynamic obstacles** with live path recomputation
- **Comparison mode** — run two algorithms side-by-side
- **Replay system** with play / pause / reverse / slow-motion / scrub
- **Stats dashboard**: nodes visited, path length, exec time, FPS, memory estimate
- **Save / load** maps as JSON
- **AI agent** that chases a target while obstacles change
- **Themes**: Samurai Neon, Matrix, Tron, Dark Minimal
- **Camera**: pan (middle-mouse drag), zoom (wheel), minimap
- **Fullscreen, resizable**, 60+ FPS, delta-time animation loop
- Clean **modular OOP** architecture, no external assets required to run

---

## Project layout

```
RoninPathfinder/
├── assets/                 (optional fonts, place a .ttf here for crisper text)
├── include/
│   ├── algorithms/         pathfinding + maze algorithms
│   ├── engine/             application, camera, clock, themes
│   ├── grid/               grid, node, terrain
│   ├── graph/              graph + edge
│   ├── ui/                 ImGui panels
│   └── utilities/          json save/load, stats, replay
├── src/                    mirrors include/, plus main.cpp
├── maps/                   sample saved maps
├── shaders/                (reserved for future glow shader)
├── CMakeLists.txt
└── README.md
```

## Build (Windows / PowerShell / VS Code)

Prerequisites:
- **Visual Studio 2022 Build Tools** (MSVC v143 + Windows SDK) — or VS Community
- **CMake 3.16+** on `PATH`
- **Git** on `PATH`
- **VS Code** with the *C/C++* and *CMake Tools* extensions

All third-party libraries (SFML, ImGui, ImGui-SFML, nlohmann/json) are fetched
automatically by CMake's `FetchContent` — **no vcpkg or manual install required**.

```powershell
# from the repo root
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Run
.\build\Release\RoninPathfinder.exe
```

Or, inside **VS Code**:
1. `Ctrl+Shift+P` → **CMake: Configure** → pick *Visual Studio 2022 Release - amd64*
2. `Ctrl+Shift+P` → **CMake: Build**
3. `F5` to debug, or run the produced exe in `build/Release/`.

### Build on Linux / macOS

```bash
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev \
                 libxi-dev libudev-dev libfreetype-dev   # Linux only
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/RoninPathfinder
```

---

## Controls

| Input | Action |
|------|--------|
| **Left click** | Place obstacle / paint current terrain / set node |
| **Right click** | Remove obstacle / erase |
| **Drag (LMB)** | Continuous paint |
| **Middle-mouse drag** | Pan camera |
| **Mouse wheel** | Zoom |
| **S** then click | Set Start node |
| **E** then click | Set End node |
| **Space** | Start / pause simulation |
| **R** | Reset traversal |
| **M** | Generate maze |
| **C** | Clear grid |
| **Tab** | Toggle Grid ⇄ Graph mode |
| **F11** | Toggle fullscreen |

Everything else (algorithm choice, heuristic, terrain brush, comparison mode,
replay, themes, save/load) is exposed through the ImGui side panel.

---

## Architecture notes

- `engine::Application` owns the SFML window, ImGui context, theme, camera,
  and the active `Simulation`.
- `algorithms::IPathfinder` is the polymorphic base class — every algorithm
  implements `step()` so the visualization loop can advance one expansion
  per frame and integrate seamlessly with pause/replay/comparison.
- `grid::Grid` stores nodes contiguously in a `std::vector<Node>` for cache
  locality; neighbor lookup is O(1).
- `utilities::Replay` records (`nodeIndex`, `state`) tuples per algorithm step,
  enabling reverse playback without re-running the algorithm.
- `ui::UiManager` is the only translation unit that touches ImGui — keeps the
  rest of the engine UI-agnostic and unit-testable.

Have fun. 侍
