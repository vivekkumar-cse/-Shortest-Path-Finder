#pragma once
#include "algorithms/IPathfinder.h"
#include <queue>
#include <vector>

namespace ronin::algorithms {

// Unified priority-queue based pathfinder. Algorithm chosen by `mode`.
class PriorityPathfinder : public IPathfinder {
public:
    enum class Mode { Dijkstra, AStar, Greedy };
    explicit PriorityPathfinder(Mode m) : mode_(m) {}
    const char* name() const override {
        switch (mode_) {
            case Mode::Dijkstra: return "Dijkstra";
            case Mode::AStar:    return "A*";
            case Mode::Greedy:   return "Greedy Best-First";
        }
        return "?";
    }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;

private:
    struct Entry { float key; int idx; };
    struct Cmp { bool operator()(const Entry& a, const Entry& b) const { return a.key > b.key; } };
    std::priority_queue<Entry, std::vector<Entry>, Cmp> open_;
    PathfinderConfig cfg_;
    Mode mode_;
};

} // namespace ronin::algorithms
