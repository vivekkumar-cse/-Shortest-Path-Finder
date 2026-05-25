#include "graph/Graph.h"
#include <queue>
#include <cmath>
#include <algorithm>

namespace ronin::graph {

int Graph::addNode(sf::Vector2f p) {
    int id = static_cast<int>(nodes_.size());
    nodes_.push_back({id, p});
    adj_[id] = {};
    return id;
}

void Graph::addEdge(int a, int b, float w) {
    if (!has(a) || !has(b) || a == b)
        return;

    adj_[a].push_back({b, w});
    adj_[b].push_back({a, w});
}

void Graph::clear() {
    nodes_.clear();
    adj_.clear();
    start = end = -1;
}

int Graph::hitTest(sf::Vector2f p, float r) const {
    for (const auto& n : nodes_) {
        sf::Vector2f d = n.pos - p;

        if (d.x * d.x + d.y * d.y <= r * r)
            return n.id;
    }

    return -1;
}

void Graph::runDijkstra() {

    // Reset nodes
    for (auto& n : nodes_) {
        n.g = 1e30f;
        n.parent = -1;

        if (n.state != Node::Start &&
            n.state != Node::End)
        {
            n.state = Node::Idle;
        }
    }

    if (start < 0 || end < 0)
        return;

    using Q = std::pair<float, int>;

    std::priority_queue<
        Q,
        std::vector<Q>,
        std::greater<Q>
    > pq;

    nodes_[start].g = 0.f;
    pq.push({0.f, start});

    while (!pq.empty()) {

        auto [d, u] = pq.top();
        pq.pop();

        if (d > nodes_[u].g)
            continue;

        if (u == end)
            break;

        if (nodes_[u].state != Node::Start &&
            nodes_[u].state != Node::End)
        {
            nodes_[u].state = Node::Closed;
        }

        for (const auto& e : adj_[u]) {

            float nd = d + e.w;

            if (nd < nodes_[e.to].g) {

                nodes_[e.to].g = nd;
                nodes_[e.to].parent = u;

                pq.push({nd, e.to});
            }
        }
    }

    // Reconstruct path
    int cur = end;

    while (cur != -1 && cur != start) {

        if (nodes_[cur].state != Node::End) {
            nodes_[cur].state = Node::Path;
        }

        cur = nodes_[cur].parent;
    }
}

void Graph::draw(sf::RenderTarget& t, sf::RenderStates s) const {

    // Draw edges
    sf::VertexArray lines(sf::Lines);

    for (const auto& [id, es] : adj_) {

        for (const auto& e : es) {

            if (e.to < id)
                continue;

            sf::Color c(80, 110, 140, 200);

            if (
                (nodes_[id].state == Node::Path ||
                 nodes_[id].state == Node::Start) &&

                (nodes_[e.to].state == Node::Path ||
                 nodes_[e.to].state == Node::End)
            )
            {
                c = sf::Color(255, 220, 80, 255);
            }

            lines.append(sf::Vertex(nodes_[id].pos, c));
            lines.append(sf::Vertex(nodes_[e.to].pos, c));
        }
    }

    t.draw(lines, s);

    // Draw nodes
    for (const auto& n : nodes_) {

        sf::CircleShape c(10.f);

        c.setOrigin(10.f, 10.f);
        c.setPosition(n.pos);

        sf::Color col(70, 130, 180);

        switch (n.state) {

            case Node::Start:
                col = sf::Color(80, 255, 130);
                break;

            case Node::End:
                col = sf::Color(255, 90, 140);
                break;

            case Node::Path:
                col = sf::Color(255, 220, 80);
                break;

            case Node::Closed:
                col = sf::Color(40, 90, 140);
                break;

            case Node::Open:
                col = sf::Color(80, 200, 220);
                break;

            default:
                break;
        }

        c.setFillColor(col);
        c.setOutlineColor(sf::Color(220, 220, 230));
        c.setOutlineThickness(2.f);

        t.draw(c, s);
    }
}

} // namespace ronin::graph