#pragma once
#include "grid/Terrain.h"
#include <limits>
#include <cstdint>

namespace ronin::grid {

enum class NodeState : unsigned char {
    Idle,
    Open,
    Closed,
    Path,
    Start,
    End
};

struct Node {
    int x = 0;
    int y = 0;
    Terrain terrain = Terrain::Empty;
    NodeState state = NodeState::Idle;
    float g = std::numeric_limits<float>::infinity();
    float h = 0.f;
    float f = std::numeric_limits<float>::infinity();
    int parent = -1; // index into grid
    bool visited = false;
    float animT = 0.f; // animation time accumulator
};

} // namespace ronin::grid
