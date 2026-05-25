#include "algorithms/Algorithms.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace ronin::algorithms {

// ===================== BFS =====================
void BFS::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    while (!q_.empty()) q_.pop();
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    g.at(g.start()).g = 0;
    g.at(g.start()).visited = true;
    q_.push(g.start());
}

bool BFS::step(grid::Grid& g) {
    if (finished_ || q_.empty()) { finished_ = true; return false; }
    int cur = q_.front(); q_.pop();
    visitedCount_++;
    auto& c = g.at(cur);
    if (c.state != grid::NodeState::Start && c.state != grid::NodeState::End)
        c.state = grid::NodeState::Closed;
    if (cur == g.end()) { finished_ = true; succeeded_ = true; return false; }
    std::array<int, 8> nbrs{};
    int n = g.neighbors(cur, cfg_.allowDiagonal, nbrs);
    for (int k = 0; k < n; ++k) {
        int ni = nbrs[k];
        auto& nb = g.at(ni);
        if (nb.visited) continue;
        nb.visited = true;
        nb.parent = cur;
        nb.g = c.g + 1;
        if (nb.state != grid::NodeState::Start && nb.state != grid::NodeState::End)
            nb.state = grid::NodeState::Open;
        q_.push(ni);
    }
    return true;
}

// ===================== DFS =====================
void DFS::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    while (!s_.empty()) s_.pop();
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    s_.push(g.start());
}

bool DFS::step(grid::Grid& g) {
    if (finished_ || s_.empty()) { finished_ = true; return false; }
    int cur = s_.top(); s_.pop();
    auto& c = g.at(cur);
    if (c.visited) return true;
    c.visited = true;
    visitedCount_++;
    if (c.state != grid::NodeState::Start && c.state != grid::NodeState::End)
        c.state = grid::NodeState::Closed;
    if (cur == g.end()) { finished_ = true; succeeded_ = true; return false; }
    std::array<int, 8> nbrs{};
    int n = g.neighbors(cur, cfg_.allowDiagonal, nbrs);
    for (int k = 0; k < n; ++k) {
        int ni = nbrs[k];
        auto& nb = g.at(ni);
        if (nb.visited) continue;
        if (nb.parent == -1) nb.parent = cur;
        if (nb.state != grid::NodeState::Start && nb.state != grid::NodeState::End)
            nb.state = grid::NodeState::Open;
        s_.push(ni);
    }
    return true;
}

// ===================== Bellman-Ford =====================
void BellmanFord::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    iter_ = 0; total_ = g.size() - 1;
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    g.at(g.start()).g = 0;
}

bool BellmanFord::step(grid::Grid& g) {
    if (finished_) return false;
    if (iter_ >= total_) {
        finished_ = true;
        succeeded_ = std::isfinite(g.at(g.end()).g);
        return false;
    }
    bool changed = false;
    for (int i = 0; i < g.size(); ++i) {
        if (!std::isfinite(g.at(i).g)) continue;
        std::array<int, 8> nbrs{};
        int n = g.neighbors(i, cfg_.allowDiagonal, nbrs);
        for (int k = 0; k < n; ++k) {
            int ni = nbrs[k];
            float w = g.moveCost(i, ni);
            if (g.at(i).g + w < g.at(ni).g) {
                g.at(ni).g = g.at(i).g + w;
                g.at(ni).parent = i;
                changed = true;
                if (g.at(ni).state != grid::NodeState::Start && g.at(ni).state != grid::NodeState::End)
                    g.at(ni).state = grid::NodeState::Open;
            }
        }
    }
    visitedCount_++;
    iter_++;
    if (!changed) {
        finished_ = true;
        succeeded_ = std::isfinite(g.at(g.end()).g);
        return false;
    }
    return true;
}

// ===================== Bidirectional BFS =====================
void BidirectionalBFS::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    while (!qa_.empty()) qa_.pop();
    while (!qb_.empty()) qb_.pop();
    sideA_.assign(g.size(), 0);
    sideB_.assign(g.size(), 0);
    parentB_.assign(g.size(), -1);
    meet_ = -1;
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    qa_.push(g.start()); sideA_[g.start()] = 1;
    qb_.push(g.end());   sideB_[g.end()]   = 1;
}

bool BidirectionalBFS::step(grid::Grid& g) {
    if (finished_) return false;
    auto expand = [&](std::queue<int>& q,
                      std::vector<unsigned char>& self,
                      std::vector<unsigned char>& other,
                      bool isB) -> bool {
        if (q.empty()) return false;
        int cur = q.front(); q.pop();
        visitedCount_++;
        auto& c = g.at(cur);
        if (c.state != grid::NodeState::Start && c.state != grid::NodeState::End)
            c.state = grid::NodeState::Closed;
        std::array<int, 8> nbrs{};
        int n = g.neighbors(cur, cfg_.allowDiagonal, nbrs);
        for (int k = 0; k < n; ++k) {
            int ni = nbrs[k];
            if (self[ni]) continue;
            self[ni] = 1;
            if (isB) parentB_[ni] = cur;
            else     g.at(ni).parent = cur;
            if (g.at(ni).state != grid::NodeState::Start && g.at(ni).state != grid::NodeState::End)
                g.at(ni).state = grid::NodeState::Open;
            if (other[ni]) { meet_ = ni; return true; }
            q.push(ni);
        }
        return false;
    };
    bool met = expand(qa_, sideA_, sideB_, false) || expand(qb_, sideB_, sideA_, true);
    if (met) {
        // Splice parents from end-side onto start-side at meeting point.
        int cur = meet_;
        while (parentB_[cur] != -1) {
            int p = parentB_[cur];
            g.at(p).parent = cur; // reverse linkage so reconstruct from end works
            cur = p;
        }
        // Make sure end's parent chain reaches meet_
        if (g.end() != meet_) {
            int e = g.end();
            // Walk via parentB_ from end back to meet_
            // Re-walk to set parents going from end toward meet_
            int back = g.end();
            while (back != meet_ && parentB_[back] != -1) {
                int p = parentB_[back];
                g.at(back).parent = p;
                back = p;
            }
            (void)e;
        }
        finished_ = true; succeeded_ = true;
        return false;
    }
    if (qa_.empty() && qb_.empty()) { finished_ = true; return false; }
    return true;
}

// ===================== Floyd-Warshall (small grids only) =====================
void FloydWarshall::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    cursor_ = 0; reveal_.clear();
    int N = g.size();
    if (g.start() < 0 || g.end() < 0 || N > 60 * 60) {
        // Refuse on large grids; mark failed.
        finished_ = true; return;
    }
    std::vector<float> dist(static_cast<size_t>(N) * N, std::numeric_limits<float>::infinity());
    next_.assign(static_cast<size_t>(N) * N, -1);
    for (int i = 0; i < N; ++i) {
        if (g.at(i).terrain == grid::Terrain::Wall) continue;
        dist[i * N + i] = 0;
        std::array<int, 8> nbrs{};
        int n = g.neighbors(i, cfg_.allowDiagonal, nbrs);
        for (int k = 0; k < n; ++k) {
            int j = nbrs[k];
            dist[i * N + j] = g.moveCost(i, j);
            next_[i * N + j] = j;
        }
    }
    for (int k = 0; k < N; ++k)
        for (int i = 0; i < N; ++i) {
            float dik = dist[i * N + k];
            if (!std::isfinite(dik)) continue;
            for (int j = 0; j < N; ++j) {
                float nd = dik + dist[k * N + j];
                if (nd < dist[i * N + j]) {
                    dist[i * N + j] = nd;
                    next_[i * N + j] = next_[i * N + k];
                }
            }
        }
    // Reconstruct path via next_
    int s = g.start(), e = g.end();
    if (next_[s * N + e] == -1) { finished_ = true; return; }
    int cur = s;
    while (cur != e) {
        reveal_.push_back(cur);
        cur = next_[cur * N + e];
        if (cur == -1) { finished_ = true; return; }
    }
    reveal_.push_back(e);
    // build parents
    for (size_t i = 1; i < reveal_.size(); ++i)
        g.at(reveal_[i]).parent = reveal_[i - 1];
    succeeded_ = true;
}

bool FloydWarshall::step(grid::Grid& g) {
    if (finished_) return false;
    if (cursor_ >= reveal_.size()) { finished_ = true; return false; }
    int idx = reveal_[cursor_++];
    visitedCount_++;
    auto& n = g.at(idx);
    if (n.state != grid::NodeState::Start && n.state != grid::NodeState::End)
        n.state = grid::NodeState::Closed;
    return cursor_ < reveal_.size();
}

// ===================== Jump Point Search (simplified) =====================
void JumpPointSearch::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg; cfg_.allowDiagonal = true;
    finished_ = succeeded_ = false; visitedCount_ = pathLength_ = 0; path_.clear();
    while (!open_.empty()) open_.pop();
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    auto& s = g.at(g.start());
    s.g = 0;
    s.f = heuristic(cfg_.heuristic, s.x, s.y, g.at(g.end()).x, g.at(g.end()).y);
    open_.push({s.f, g.start()});
}

int JumpPointSearch::jump(grid::Grid& g, int x, int y, int dx, int dy) const {
    int nx = x + dx, ny = y + dy;
    if (!g.inBounds(nx, ny)) return -1;
    if (g.at(nx, ny).terrain == grid::Terrain::Wall) return -1;
    int idx = g.index(nx, ny);
    if (idx == g.end()) return idx;
    // Forced neighbor check (simplified)
    if (dx != 0 && dy != 0) {
        if ((g.inBounds(nx - dx, ny + dy) && g.at(nx - dx, ny).terrain == grid::Terrain::Wall) ||
            (g.inBounds(nx + dx, ny - dy) && g.at(nx, ny - dy).terrain == grid::Terrain::Wall))
            return idx;
        if (jump(g, nx, ny, dx, 0) != -1) return idx;
        if (jump(g, nx, ny, 0, dy) != -1) return idx;
    } else {
        if (dx != 0) {
            if ((g.inBounds(nx + dx, ny + 1) && g.at(nx, ny + 1).terrain == grid::Terrain::Wall) ||
                (g.inBounds(nx + dx, ny - 1) && g.at(nx, ny - 1).terrain == grid::Terrain::Wall))
                return idx;
        } else {
            if ((g.inBounds(nx + 1, ny + dy) && g.at(nx + 1, ny).terrain == grid::Terrain::Wall) ||
                (g.inBounds(nx - 1, ny + dy) && g.at(nx - 1, ny).terrain == grid::Terrain::Wall))
                return idx;
        }
    }
    return jump(g, nx, ny, dx, dy);
}

bool JumpPointSearch::step(grid::Grid& g) {
    if (finished_) return false;
    if (open_.empty()) { finished_ = true; return false; }
    auto e = open_.top(); open_.pop();
    auto& cur = g.at(e.idx);
    if (cur.visited) return true;
    cur.visited = true;
    visitedCount_++;
    if (cur.state != grid::NodeState::Start && cur.state != grid::NodeState::End)
        cur.state = grid::NodeState::Closed;
    if (e.idx == g.end()) { finished_ = true; succeeded_ = true; return false; }
    static const int dx8[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    static const int dy8[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    const auto& end = g.at(g.end());
    for (int d = 0; d < 8; ++d) {
        int ji = jump(g, cur.x, cur.y, dx8[d], dy8[d]);
        if (ji < 0) continue;
        auto& jn = g.at(ji);
        if (jn.visited) continue;
        float dx = static_cast<float>(jn.x - cur.x);
        float dy = static_cast<float>(jn.y - cur.y);
        float step = std::sqrt(dx * dx + dy * dy);
        float ng = cur.g + step;
        if (ng < jn.g) {
            jn.g = ng;
            jn.parent = e.idx;
            jn.h = heuristic(cfg_.heuristic, jn.x, jn.y, end.x, end.y);
            jn.f = jn.g + jn.h;
            if (jn.state != grid::NodeState::Start && jn.state != grid::NodeState::End)
                jn.state = grid::NodeState::Open;
            open_.push({jn.f, ji});
        }
    }
    return true;
}

} // namespace ronin::algorithms
