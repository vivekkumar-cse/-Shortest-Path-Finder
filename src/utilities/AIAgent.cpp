#include "utilities/AIAgent.h"
#include <queue>
#include <array>
#include <cstdlib>

namespace ronin::utilities {

static std::vector<int> bfsPath(grid::Grid& g, int s, int e) {
    if (s < 0 || e < 0) return {};
    std::vector<int> parent(g.size(), -1);
    std::vector<unsigned char> seen(g.size(), 0);
    std::queue<int> q; q.push(s); seen[s] = 1;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == e) break;
        std::array<int, 8> nb{};
        int n = g.neighbors(u, true, nb);
        for (int k = 0; k < n; ++k) {
            int v = nb[k];
            if (seen[v]) continue;
            seen[v] = 1; parent[v] = u; q.push(v);
        }
    }
    std::vector<int> path;
    if (!seen[e]) return path;
    for (int c = e; c != -1; c = parent[c]) path.push_back(c);
    std::reverse(path.begin(), path.end());
    return path;
}

void AIAgent::update(grid::Grid& g, int target, float dt) {
    if (node_ < 0 || target < 0) return;
    timer_ += dt;
    // Re-plan every 0.5s or when path is empty
    if (path_.empty() || cursor_ >= path_.size() || timer_ > 0.5f) {
        path_ = bfsPath(g, node_, target);
        cursor_ = 0;
        timer_ = 0.f;
    }
    if (path_.size() > 1 && cursor_ + 1 < path_.size()) {
        node_ = path_[++cursor_];
    }
}

void AIAgent::draw(sf::RenderTarget& t, const grid::Grid& g) const {
    if (node_ < 0) return;
    const auto& n = g.at(node_);
    float c = g.cellSize();
    sf::CircleShape s(c * 0.4f);
    s.setOrigin(c * 0.4f, c * 0.4f);
    s.setPosition(n.x * c + c * 0.5f, n.y * c + c * 0.5f);
    s.setFillColor(sf::Color(255, 255, 0, 230));
    s.setOutlineColor(sf::Color(255, 200, 0));
    s.setOutlineThickness(2.f);
    t.draw(s);
}

void DynamicObstacleField::spawn(grid::Grid& g, int count) {
    clear(g);
    for (int i = 0; i < count; ++i) {
        int x = 2 + std::rand() % std::max(1, g.width() - 4);
        int y = 2 + std::rand() % std::max(1, g.height() - 4);
        int dx = (std::rand() % 2) ? 1 : -1;
        int dy = (std::rand() % 2) ? 1 : -1;
        if (g.at(x, y).terrain == grid::Terrain::Empty) {
            obstacles_.push_back({x, y, dx, dy});
            g.at(x, y).terrain = grid::Terrain::Wall;
        }
    }
}

void DynamicObstacleField::update(grid::Grid& g, float dt) {
    timer_ += dt;
    if (timer_ < 0.15f) return;
    timer_ = 0;
    for (auto& o : obstacles_) {
        if (g.inBounds(o.x, o.y) && g.at(o.x, o.y).terrain == grid::Terrain::Wall &&
            g.at(o.x, o.y).state != grid::NodeState::Start &&
            g.at(o.x, o.y).state != grid::NodeState::End)
            g.at(o.x, o.y).terrain = grid::Terrain::Empty;
        int nx = o.x + o.dx, ny = o.y + o.dy;
        if (!g.inBounds(nx, ny) || g.at(nx, ny).terrain == grid::Terrain::Wall) {
            o.dx = -o.dx; o.dy = -o.dy;
            nx = o.x + o.dx; ny = o.y + o.dy;
            if (!g.inBounds(nx, ny)) continue;
        }
        o.x = nx; o.y = ny;
        if (g.at(o.x, o.y).state != grid::NodeState::Start &&
            g.at(o.x, o.y).state != grid::NodeState::End)
            g.at(o.x, o.y).terrain = grid::Terrain::Wall;
    }
}

void DynamicObstacleField::clear(grid::Grid& g) {
    for (auto& o : obstacles_) {
        if (g.inBounds(o.x, o.y) && g.at(o.x, o.y).terrain == grid::Terrain::Wall)
            g.at(o.x, o.y).terrain = grid::Terrain::Empty;
    }
    obstacles_.clear();
}

} // namespace ronin::utilities
