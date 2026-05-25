#pragma once
#include "algorithms/IPathfinder.h"
#include <queue>
#include <stack>

namespace ronin::algorithms {

class BFS : public IPathfinder {
public:
    const char* name() const override { return "BFS"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    std::queue<int> q_;
    PathfinderConfig cfg_;
};

class DFS : public IPathfinder {
public:
    const char* name() const override { return "DFS"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    std::stack<int> s_;
    PathfinderConfig cfg_;
};

class BellmanFord : public IPathfinder {
public:
    const char* name() const override { return "Bellman-Ford"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    int iter_ = 0;
    int total_ = 0;
    PathfinderConfig cfg_;
};

class BidirectionalBFS : public IPathfinder {
public:
    const char* name() const override { return "Bidirectional BFS"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    std::queue<int> qa_, qb_;
    std::vector<int> parentB_;
    std::vector<unsigned char> sideA_, sideB_;
    int meet_ = -1;
    PathfinderConfig cfg_;
};

// Floyd-Warshall only runs once at init (O(V^3)); steps just animate path.
class FloydWarshall : public IPathfinder {
public:
    const char* name() const override { return "Floyd-Warshall"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    PathfinderConfig cfg_;
    std::vector<int> next_;
    std::vector<int> reveal_;
    size_t cursor_ = 0;
};

// JPS: simplified variant on uniform-cost grids (treats weights as 1, prunes).
class JumpPointSearch : public IPathfinder {
public:
    const char* name() const override { return "Jump Point Search"; }
    void init(grid::Grid& g, const PathfinderConfig& cfg) override;
    bool step(grid::Grid& g) override;
private:
    struct Entry { float f; int idx; };
    struct Cmp { bool operator()(const Entry& a, const Entry& b) const { return a.f > b.f; } };
    std::priority_queue<Entry, std::vector<Entry>, Cmp> open_;
    PathfinderConfig cfg_;
    int jump(grid::Grid& g, int x, int y, int dx, int dy) const;
};

} // namespace ronin::algorithms
