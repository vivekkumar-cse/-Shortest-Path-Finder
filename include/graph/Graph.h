#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>

namespace ronin::graph {

struct Node {
    int id;
    sf::Vector2f pos;
    enum State { Idle, Open, Closed, Path, Start, End } state = Idle;
    float g = 1e30f;
    int parent = -1;
};

struct Edge {
    int to;
    float w;
};

class Graph : public sf::Drawable {
public:
    int addNode(sf::Vector2f p);
    void addEdge(int a, int b, float w);
    void clear();

    std::vector<Node>& nodes() { return nodes_; }
    const std::vector<Node>& nodes() const { return nodes_; }
    const std::vector<Edge>& neighbors(int id) const { return adj_.at(id); }
    bool has(int id) const { return id >= 0 && id < (int)nodes_.size(); }

    int start = -1, end = -1;
    int hitTest(sf::Vector2f p, float radius = 14.f) const;
    void runDijkstra();

protected:
    void draw(sf::RenderTarget& t, sf::RenderStates s) const override;
private:
    std::vector<Node> nodes_;
    std::unordered_map<int, std::vector<Edge>> adj_;
};

} // namespace ronin::graph
