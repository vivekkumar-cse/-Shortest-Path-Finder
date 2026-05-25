#pragma once
#include "grid/Node.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

namespace ronin::grid {

class Grid : public sf::Drawable {
public:
    Grid(int w = 60, int h = 40);

    void resize(int w, int h);
    void clearAll();
    void clearTraversal();

    int width()  const { return w_; }
    int height() const { return h_; }
    int size()   const { return w_ * h_; }

    int  index(int x, int y) const { return y * w_ + x; }
    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < w_ && y < h_; }

    Node&       at(int x, int y)       { return nodes_[index(x, y)]; }
    const Node& at(int x, int y) const { return nodes_[index(x, y)]; }
    Node&       at(int i)              { return nodes_[i]; }
    const Node& at(int i) const        { return nodes_[i]; }

    std::vector<Node>&       data()       { return nodes_; }
    const std::vector<Node>& data() const { return nodes_; }

    int start() const { return start_; }
    int end()   const { return end_; }
    void setStart(int i);
    void setEnd(int i);

    // returns up to 8 neighbors; pass allowDiagonal
    int neighbors(int i, bool allowDiagonal, std::array<int, 8>& out) const;
    float moveCost(int from, int to) const;

    float cellSize() const { return cell_; }
    void  setCellSize(float c) { cell_ = c; }

    void update(float dt);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    int w_, h_;
    float cell_ = 24.f;
    std::vector<Node> nodes_;
    int start_ = -1;
    int end_   = -1;
};

} // namespace ronin::grid
