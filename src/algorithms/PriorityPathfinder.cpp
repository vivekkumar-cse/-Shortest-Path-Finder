#include "algorithms/PriorityPathfinder.h"

namespace ronin::algorithms {

void PriorityPathfinder::init(grid::Grid& g, const PathfinderConfig& cfg) {
    cfg_ = cfg;
    finished_ = false; succeeded_ = false;
    visitedCount_ = 0; pathLength_ = 0; path_.clear();
    while (!open_.empty()) open_.pop();
    if (g.start() < 0 || g.end() < 0) { finished_ = true; return; }
    auto& s = g.at(g.start());
    s.g = 0;
    s.h = heuristic(cfg_.heuristic, s.x, s.y, g.at(g.end()).x, g.at(g.end()).y);
    s.f = (mode_ == Mode::Greedy) ? s.h : s.g + s.h;
    open_.push({s.f, g.start()});
}

bool PriorityPathfinder::step(grid::Grid& g) {
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

    std::array<int, 8> nbrs{};
    int n = g.neighbors(e.idx, cfg_.allowDiagonal, nbrs);
    int endIdx = g.end();
    const auto& end = g.at(endIdx);
    for (int k = 0; k < n; ++k) {
        int ni = nbrs[k];
        auto& nb = g.at(ni);
        if (nb.visited) continue;
        float tentative = cur.g + g.moveCost(e.idx, ni);
        if (tentative < nb.g) {
            nb.parent = e.idx;
            nb.g = tentative;
            nb.h = heuristic(cfg_.heuristic, nb.x, nb.y, end.x, end.y);
            switch (mode_) {
                case Mode::Dijkstra: nb.f = nb.g; break;
                case Mode::AStar:    nb.f = nb.g + nb.h; break;
                case Mode::Greedy:   nb.f = nb.h; break;
            }
            open_.push({nb.f, ni});
            if (nb.state != grid::NodeState::Start && nb.state != grid::NodeState::End)
                nb.state = grid::NodeState::Open;
        }
    }
    return true;
}

} // namespace ronin::algorithms
