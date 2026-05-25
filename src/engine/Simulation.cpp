#include "engine/Simulation.h"
#include "algorithms/PriorityPathfinder.h"
#include "algorithms/Algorithms.h"
#include <chrono>

namespace ronin::engine {

static double nowMs() {
    using namespace std::chrono;
    return duration<double, std::milli>(steady_clock::now().time_since_epoch()).count();
}

Simulation::Simulation() : grid_(60, 40) {
    selectAlgorithm(AlgorithmId::AStar);
    selectComparisonAlgorithm(AlgorithmId::Dijkstra);
}

std::unique_ptr<algorithms::IPathfinder> Simulation::makeFinder(AlgorithmId id) {
    using namespace algorithms;
    switch (id) {
        case AlgorithmId::Dijkstra:         return std::make_unique<PriorityPathfinder>(PriorityPathfinder::Mode::Dijkstra);
        case AlgorithmId::AStar:            return std::make_unique<PriorityPathfinder>(PriorityPathfinder::Mode::AStar);
        case AlgorithmId::Greedy:           return std::make_unique<PriorityPathfinder>(PriorityPathfinder::Mode::Greedy);
        case AlgorithmId::BFS:              return std::make_unique<BFS>();
        case AlgorithmId::DFS:              return std::make_unique<DFS>();
        case AlgorithmId::BellmanFord:      return std::make_unique<BellmanFord>();
        case AlgorithmId::BidirectionalBFS: return std::make_unique<BidirectionalBFS>();
        case AlgorithmId::FloydWarshall:    return std::make_unique<FloydWarshall>();
        case AlgorithmId::JPS:              return std::make_unique<JumpPointSearch>();
    }
    return std::make_unique<PriorityPathfinder>(PriorityPathfinder::Mode::AStar);
}

void Simulation::selectAlgorithm(AlgorithmId id) {
    currentAlgo_ = id;
    finder_ = makeFinder(id);
    stats_.algorithmName = finder_->name();
    stats_.complexity = utilities::complexityFor(finder_->name());
    resetTraversal();
}
void Simulation::selectComparisonAlgorithm(AlgorithmId id) {
    compareAlgo_ = id;
    compareFinder_ = makeFinder(id);
    compareStats_.algorithmName = compareFinder_->name();
    compareStats_.complexity = utilities::complexityFor(compareFinder_->name());
}

void Simulation::resetTraversal() {
    grid_.clearTraversal();
    if (finder_)        finder_->init(grid_, cfg_);
    if (compareFinder_) compareFinder_->init(grid_, cfg_);
    stats_.nodesVisited = 0; stats_.pathLength = 0; stats_.execTimeMs = 0;
    compareStats_.nodesVisited = 0; compareStats_.pathLength = 0; compareStats_.execTimeMs = 0;
    replay_.clear();
    startMs_ = 0;
}

void Simulation::clearAll() {
    grid_.clearAll();
    dynObs_.clear(grid_);
    resetTraversal();
}

void Simulation::startMaze(int kind) {
    using namespace algorithms;
    switch (kind) {
        case 0: mazeGen_ = std::make_unique<RecursiveBacktracker>(); break;
        case 1: mazeGen_ = std::make_unique<PrimMaze>(); break;
        case 2: mazeGen_ = std::make_unique<KruskalMaze>(); break;
        default: return;
    }
    mazeGen_->init(grid_, 0);
    resetTraversal();
}

void Simulation::update(float dt) {
    grid_.update(dt);

    if (mazeGen_) {
        // Several maze steps per frame for snappy generation.
        for (int i = 0; i < 12; ++i) {
            if (!mazeGen_->step(grid_)) { mazeGen_.reset(); break; }
        }
        return;
    }

    if (dynamicObstaclesEnabled) dynObs_.update(grid_, dt);

    if (agentEnabled) agent_.update(grid_, grid_.end(), dt);

    if (!running_) return;
    if (!finder_) return;

    if (startMs_ == 0.0) startMs_ = nowMs();

    stepAccumulator_ += dt * stepsPerSecond;
    int steps = static_cast<int>(stepAccumulator_);
    stepAccumulator_ -= steps;

    for (int i = 0; i < steps; ++i) {
        bool a = !finder_->finished() && finder_->step(grid_);
        bool b = comparisonEnabled && compareFinder_ && !compareFinder_->finished()
                  && compareFinder_->step(grid_);
        (void)a; (void)b;
        if (finder_->finished() && (!comparisonEnabled || !compareFinder_ || compareFinder_->finished())) {
            if (finder_->succeeded()) finder_->reconstruct(grid_);
            if (comparisonEnabled && compareFinder_ && compareFinder_->succeeded())
                compareFinder_->reconstruct(grid_);
            stats_.execTimeMs = nowMs() - startMs_;
            stats_.nodesVisited = finder_->visited();
            stats_.pathLength = finder_->pathLength();
            if (comparisonEnabled && compareFinder_) {
                compareStats_.execTimeMs = nowMs() - startMs_;
                compareStats_.nodesVisited = compareFinder_->visited();
                compareStats_.pathLength = compareFinder_->pathLength();
            }
            running_ = false;
            break;
        }
    }

    stats_.nodesVisited = finder_->visited();
    if (comparisonEnabled && compareFinder_)
        compareStats_.nodesVisited = compareFinder_->visited();
}

} // namespace ronin::engine
