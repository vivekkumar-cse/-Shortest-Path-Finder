#pragma once
#include "grid/Grid.h"
#include <vector>
#include <random>

namespace ronin::algorithms {

// Animated maze generators. Each implements a step() interface like algorithms.
class IMazeGenerator {
public:
    virtual ~IMazeGenerator() = default;
    virtual const char* name() const = 0;
    virtual void init(grid::Grid& g, uint32_t seed = 0) = 0;
    virtual bool step(grid::Grid& g) = 0;
    bool finished() const { return finished_; }
protected:
    bool finished_ = false;
    std::mt19937 rng_;
};

class RecursiveBacktracker : public IMazeGenerator {
public:
    const char* name() const override { return "Recursive Backtracking"; }
    void init(grid::Grid& g, uint32_t seed = 0) override;
    bool step(grid::Grid& g) override;
private:
    std::vector<std::pair<int,int>> stack_;
    std::vector<unsigned char> carved_;
};

class PrimMaze : public IMazeGenerator {
public:
    const char* name() const override { return "Prim's Maze"; }
    void init(grid::Grid& g, uint32_t seed = 0) override;
    bool step(grid::Grid& g) override;
private:
    std::vector<std::pair<int,int>> frontier_;
    std::vector<unsigned char> inMaze_;
};

class KruskalMaze : public IMazeGenerator {
public:
    const char* name() const override { return "Kruskal's Maze"; }
    void init(grid::Grid& g, uint32_t seed = 0) override;
    bool step(grid::Grid& g) override;
private:
    struct Edge { int a, b, wallX, wallY; };
    std::vector<Edge> edges_;
    std::vector<int> parent_;
    size_t cursor_ = 0;
    int find(int x);
    bool unite(int a, int b);
};

} // namespace ronin::algorithms
