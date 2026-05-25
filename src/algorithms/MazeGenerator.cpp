#include "algorithms/MazeGenerator.h"
#include <algorithm>

namespace ronin::algorithms {

static void fillWalls(grid::Grid& g) {
    for (auto& n : g.data()) n.terrain = grid::Terrain::Wall;
    if (g.start() >= 0) g.at(g.start()).terrain = grid::Terrain::Empty;
    if (g.end()   >= 0) g.at(g.end()).terrain   = grid::Terrain::Empty;
}

// ============ Recursive Backtracker ============
void RecursiveBacktracker::init(grid::Grid& g, uint32_t seed) {
    rng_.seed(seed ? seed : std::random_device{}());
    finished_ = false;
    fillWalls(g);
    carved_.assign(g.size(), 0);
    stack_.clear();
    int sx = 1, sy = 1;
    g.at(sx, sy).terrain = grid::Terrain::Empty;
    carved_[g.index(sx, sy)] = 1;
    stack_.push_back({sx, sy});
}

bool RecursiveBacktracker::step(grid::Grid& g) {
    if (finished_ || stack_.empty()) { finished_ = true; return false; }
    auto [x, y] = stack_.back();
    static const int dx[4] = {2, -2, 0, 0};
    static const int dy[4] = {0, 0, 2, -2};
    int order[4] = {0, 1, 2, 3};
    std::shuffle(order, order + 4, rng_);
    for (int o = 0; o < 4; ++o) {
        int d = order[o];
        int nx = x + dx[d], ny = y + dy[d];
        if (nx <= 0 || ny <= 0 || nx >= g.width() - 1 || ny >= g.height() - 1) continue;
        if (carved_[g.index(nx, ny)]) continue;
        g.at((x + nx) / 2, (y + ny) / 2).terrain = grid::Terrain::Empty;
        g.at(nx, ny).terrain = grid::Terrain::Empty;
        carved_[g.index(nx, ny)] = 1;
        stack_.push_back({nx, ny});
        return true;
    }
    stack_.pop_back();
    return !stack_.empty();
}

// ============ Prim's ============
void PrimMaze::init(grid::Grid& g, uint32_t seed) {
    rng_.seed(seed ? seed : std::random_device{}());
    finished_ = false;
    fillWalls(g);
    inMaze_.assign(g.size(), 0);
    frontier_.clear();
    int sx = 1, sy = 1;
    g.at(sx, sy).terrain = grid::Terrain::Empty;
    inMaze_[g.index(sx, sy)] = 1;
    auto pushFrontier = [&](int x, int y) {
        static const int dx[4] = {2, -2, 0, 0};
        static const int dy[4] = {0, 0, 2, -2};
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx <= 0 || ny <= 0 || nx >= g.width() - 1 || ny >= g.height() - 1) continue;
            if (inMaze_[g.index(nx, ny)]) continue;
            frontier_.push_back({nx, ny});
            inMaze_[g.index(nx, ny)] = 2; // frontier marker
        }
    };
    pushFrontier(sx, sy);
}

bool PrimMaze::step(grid::Grid& g) {
    if (finished_ || frontier_.empty()) { finished_ = true; return false; }
    std::uniform_int_distribution<size_t> dist(0, frontier_.size() - 1);
    size_t k = dist(rng_);
    auto [x, y] = frontier_[k];
    frontier_[k] = frontier_.back(); frontier_.pop_back();
    static const int dx[4] = {2, -2, 0, 0};
    static const int dy[4] = {0, 0, 2, -2};
    std::vector<int> opts;
    for (int d = 0; d < 4; ++d) {
        int nx = x + dx[d], ny = y + dy[d];
        if (nx <= 0 || ny <= 0 || nx >= g.width() - 1 || ny >= g.height() - 1) continue;
        if (inMaze_[g.index(nx, ny)] == 1) opts.push_back(d);
    }
    if (!opts.empty()) {
        int d = opts[std::uniform_int_distribution<size_t>(0, opts.size() - 1)(rng_)];
        int nx = x + dx[d], ny = y + dy[d];
        g.at(x, y).terrain = grid::Terrain::Empty;
        g.at((x + nx) / 2, (y + ny) / 2).terrain = grid::Terrain::Empty;
        inMaze_[g.index(x, y)] = 1;
        // add new frontiers from (x,y)
        for (int dd = 0; dd < 4; ++dd) {
            int fx = x + dx[dd], fy = y + dy[dd];
            if (fx <= 0 || fy <= 0 || fx >= g.width() - 1 || fy >= g.height() - 1) continue;
            if (inMaze_[g.index(fx, fy)] == 0) {
                frontier_.push_back({fx, fy});
                inMaze_[g.index(fx, fy)] = 2;
            }
        }
    }
    return !frontier_.empty();
}

// ============ Kruskal ============
int KruskalMaze::find(int x) {
    while (parent_[x] != x) { parent_[x] = parent_[parent_[x]]; x = parent_[x]; }
    return x;
}
bool KruskalMaze::unite(int a, int b) {
    a = find(a); b = find(b);
    if (a == b) return false;
    parent_[a] = b; return true;
}

void KruskalMaze::init(grid::Grid& g, uint32_t seed) {
    rng_.seed(seed ? seed : std::random_device{}());
    finished_ = false;
    fillWalls(g);
    parent_.assign(g.size(), 0);
    for (int i = 0; i < g.size(); ++i) parent_[i] = i;
    edges_.clear();
    for (int y = 1; y < g.height() - 1; y += 2)
        for (int x = 1; x < g.width() - 1; x += 2) {
            g.at(x, y).terrain = grid::Terrain::Empty;
            if (x + 2 < g.width() - 1)
                edges_.push_back({g.index(x, y), g.index(x + 2, y), x + 1, y});
            if (y + 2 < g.height() - 1)
                edges_.push_back({g.index(x, y), g.index(x, y + 2), x, y + 1});
        }
    std::shuffle(edges_.begin(), edges_.end(), rng_);
    cursor_ = 0;
}

bool KruskalMaze::step(grid::Grid& g) {
    if (finished_ || cursor_ >= edges_.size()) { finished_ = true; return false; }
    const Edge& e = edges_[cursor_++];
    if (unite(e.a, e.b))
        g.at(e.wallX, e.wallY).terrain = grid::Terrain::Empty;
    if (cursor_ >= edges_.size()) finished_ = true;
    return !finished_;
}

} // namespace ronin::algorithms
