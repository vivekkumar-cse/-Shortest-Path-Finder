#pragma once
#include "grid/Grid.h"
#include "algorithms/Heuristic.h"
#include <string>
#include <vector>

namespace ronin::algorithms {

struct PathfinderConfig {
    bool allowDiagonal = true;
    Heuristic heuristic = Heuristic::Manhattan;
};

class IPathfinder {
public:
    virtual ~IPathfinder() = default;
    virtual const char* name() const = 0;

    // Initialize for the given grid (uses grid.start() / grid.end()).
    virtual void init(grid::Grid& g, const PathfinderConfig& cfg) = 0;

    // Advance one expansion. Returns true if work remains, false if done.
    virtual bool step(grid::Grid& g) = 0;

    bool finished() const { return finished_; }
    bool succeeded() const { return succeeded_; }
    int  visited() const { return visitedCount_; }
    int  pathLength() const { return pathLength_; }

    // After finished_ && succeeded_, paints the path by setting NodeState::Path.
    void reconstruct(grid::Grid& g);

    const std::vector<int>& path() const { return path_; }

protected:
    bool finished_ = false;
    bool succeeded_ = false;
    int  visitedCount_ = 0;
    int  pathLength_ = 0;
    std::vector<int> path_;
};

} // namespace ronin::algorithms
