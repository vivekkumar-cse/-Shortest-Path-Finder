#include "grid/Grid.h"
#include <cmath>

namespace ronin::grid {

Grid::Grid(int w, int h) { resize(w, h); }

void Grid::resize(int w, int h) {
    w_ = w; h_ = h;
    nodes_.assign(static_cast<size_t>(w * h), Node{});
    for (int y = 0; y < h_; ++y)
        for (int x = 0; x < w_; ++x) {
            nodes_[index(x, y)].x = x;
            nodes_[index(x, y)].y = y;
        }
    start_ = index(2, h_ / 2);
    end_   = index(w_ - 3, h_ / 2);
    if (start_ < size()) nodes_[start_].state = NodeState::Start;
    if (end_   < size()) nodes_[end_].state   = NodeState::End;
}

void Grid::clearAll() {
    for (auto& n : nodes_) {
        n.terrain = Terrain::Empty;
        n.state = NodeState::Idle;
        n.g = std::numeric_limits<float>::infinity();
        n.h = 0; n.f = std::numeric_limits<float>::infinity();
        n.parent = -1; n.visited = false; n.animT = 0;
    }
    if (start_ >= 0) nodes_[start_].state = NodeState::Start;
    if (end_   >= 0) nodes_[end_].state   = NodeState::End;
}

void Grid::clearTraversal() {
    for (auto& n : nodes_) {
        if (n.state == NodeState::Open || n.state == NodeState::Closed || n.state == NodeState::Path)
            n.state = NodeState::Idle;
        n.g = std::numeric_limits<float>::infinity();
        n.h = 0; n.f = std::numeric_limits<float>::infinity();
        n.parent = -1; n.visited = false; n.animT = 0;
    }
    if (start_ >= 0) nodes_[start_].state = NodeState::Start;
    if (end_   >= 0) nodes_[end_].state   = NodeState::End;
}

void Grid::setStart(int i) {
    if (start_ >= 0 && nodes_[start_].state == NodeState::Start)
        nodes_[start_].state = NodeState::Idle;
    start_ = i;
    if (i >= 0) nodes_[i].state = NodeState::Start;
}

void Grid::setEnd(int i) {
    if (end_ >= 0 && nodes_[end_].state == NodeState::End)
        nodes_[end_].state = NodeState::Idle;
    end_ = i;
    if (i >= 0) nodes_[i].state = NodeState::End;
}

int Grid::neighbors(int i, bool allowDiagonal, std::array<int, 8>& out) const {
    const Node& n = nodes_[i];
    int count = 0;
    static const int dx4[4] = {1, -1, 0, 0};
    static const int dy4[4] = {0, 0, 1, -1};
    static const int dx8[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    static const int dy8[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    const int* dx = allowDiagonal ? dx8 : dx4;
    const int* dy = allowDiagonal ? dy8 : dy4;
    int k = allowDiagonal ? 8 : 4;
    for (int j = 0; j < k; ++j) {
        int nx = n.x + dx[j], ny = n.y + dy[j];
        if (!inBounds(nx, ny)) continue;
        if (at(nx, ny).terrain == Terrain::Wall) continue;
        out[count++] = index(nx, ny);
    }
    return count;
}

float Grid::moveCost(int from, int to) const {
    const Node& a = nodes_[from];
    const Node& b = nodes_[to];
    float base = (a.x != b.x && a.y != b.y) ? 1.4142f : 1.f;
    return base * terrainCost(b.terrain);
}

void Grid::update(float dt) {
    for (auto& n : nodes_) n.animT += dt;
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::VertexArray quads(sf::Quads);
    quads.resize(static_cast<size_t>(size()) * 4);
    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) {
            int i = index(x, y);
            const Node& n = nodes_[i];
            sf::Color c = terrainColor(n.terrain);
            switch (n.state) {
                case NodeState::Open:   c = {80, 200, 220, 220}; break;
                case NodeState::Closed: c = {40, 90, 140, 230};  break;
                case NodeState::Path:   c = {255, 220, 80, 255}; break;
                case NodeState::Start:  c = {80, 255, 130, 255}; break;
                case NodeState::End:    c = {255, 90, 140, 255}; break;
                default: break;
            }
            // simple pulse
            if (n.state == NodeState::Open || n.state == NodeState::Path) {
                float p = 0.5f + 0.5f * std::sin(n.animT * 6.f);
                c.r = static_cast<sf::Uint8>(std::min(255.f, c.r + p * 30.f));
                c.g = static_cast<sf::Uint8>(std::min(255.f, c.g + p * 30.f));
                c.b = static_cast<sf::Uint8>(std::min(255.f, c.b + p * 30.f));
            }
            float x0 = x * cell_, y0 = y * cell_;
            float x1 = x0 + cell_ - 1.f, y1 = y0 + cell_ - 1.f;
            size_t q = static_cast<size_t>(i) * 4;
            quads[q + 0] = sf::Vertex({x0, y0}, c);
            quads[q + 1] = sf::Vertex({x1, y0}, c);
            quads[q + 2] = sf::Vertex({x1, y1}, c);
            quads[q + 3] = sf::Vertex({x0, y1}, c);
        }
    }
    target.draw(quads, states);
}

} // namespace ronin::grid
