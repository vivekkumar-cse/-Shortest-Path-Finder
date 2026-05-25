#pragma once
#include "grid/Grid.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace ronin::utilities {

// Simple AI agent that re-plans (BFS) toward a moving target.
class AIAgent {
public:
    void setPosition(int idx) { node_ = idx; }
    int position() const { return node_; }
    void update(grid::Grid& g, int target, float dt);
    void draw(sf::RenderTarget& t, const grid::Grid& g) const;
private:
    int node_ = -1;
    std::vector<int> path_;
    size_t cursor_ = 0;
    float timer_ = 0.f;
    float speed_ = 6.f; // cells per second
    sf::Vector2f visual_{0,0};
};

// Manages dynamic obstacles bouncing around the grid.
class DynamicObstacleField {
public:
    struct Obstacle { int x, y, dx, dy; };
    void spawn(grid::Grid& g, int count);
    void update(grid::Grid& g, float dt);
    void clear(grid::Grid& g);
    const std::vector<Obstacle>& obstacles() const { return obstacles_; }
private:
    std::vector<Obstacle> obstacles_;
    float timer_ = 0.f;
};

} // namespace ronin::utilities
