#pragma once
#include "grid/Grid.h"
#include "graph/Graph.h"
#include "algorithms/IPathfinder.h"
#include "algorithms/MazeGenerator.h"
#include "utilities/Stats.h"
#include "utilities/Replay.h"
#include "utilities/AIAgent.h"
#include <memory>

namespace ronin::engine {

enum class AlgorithmId {
    Dijkstra, AStar, BFS, DFS, Greedy,
    BellmanFord, BidirectionalBFS, FloydWarshall, JPS
};

enum class Mode { Grid, Graph };

class Simulation {
public:
    Simulation();

    // ---- Grid simulation ----
    grid::Grid& grid() { return grid_; }
    const grid::Grid& grid() const { return grid_; }

    graph::Graph& graph() { return graph_; }
    const graph::Graph& graph() const { return graph_; }

    Mode mode = Mode::Grid;

    void selectAlgorithm(AlgorithmId id);
    void selectComparisonAlgorithm(AlgorithmId id);
    AlgorithmId currentAlgo() const { return currentAlgo_; }
    AlgorithmId compareAlgo() const { return compareAlgo_; }

    bool comparisonEnabled = false;

    algorithms::PathfinderConfig& config() { return cfg_; }

    void play()  { running_ = true; }
    void pause() { running_ = false; }
    bool isRunning() const { return running_; }

    void resetTraversal();
    void clearAll();

    void update(float dt);

    void startMaze(int kind); // 0=Backtrack 1=Prim 2=Kruskal
    bool mazeActive() const { return mazeGen_ != nullptr; }

    const utilities::Stats& stats() const { return stats_; }
    const utilities::Stats& compareStats() const { return compareStats_; }

    utilities::Replay& replay() { return replay_; }

    utilities::AIAgent& agent() { return agent_; }
    bool agentEnabled = false;

    utilities::DynamicObstacleField& dynObs() { return dynObs_; }
    bool dynamicObstaclesEnabled = false;

    float stepsPerSecond = 120.f;

private:
    grid::Grid grid_;
    graph::Graph graph_;

    std::unique_ptr<algorithms::IPathfinder> finder_;
    std::unique_ptr<algorithms::IPathfinder> compareFinder_;
    std::unique_ptr<algorithms::IMazeGenerator> mazeGen_;

    AlgorithmId currentAlgo_ = AlgorithmId::AStar;
    AlgorithmId compareAlgo_ = AlgorithmId::Dijkstra;

    algorithms::PathfinderConfig cfg_;
    utilities::Stats stats_, compareStats_;
    utilities::Replay replay_;
    utilities::AIAgent agent_;
    utilities::DynamicObstacleField dynObs_;

    bool running_ = false;
    double startMs_ = 0.0;
    float stepAccumulator_ = 0.f;

    static std::unique_ptr<algorithms::IPathfinder> makeFinder(AlgorithmId id);
};

} // namespace ronin::engine
