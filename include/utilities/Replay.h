#pragma once
#include "grid/Grid.h"
#include <vector>

namespace ronin::utilities {

struct ReplayFrame {
    int nodeIndex;
    grid::NodeState before;
    grid::NodeState after;
};

class Replay {
public:
    void clear() { frames_.clear(); cursor_ = 0; }
    void record(int idx, grid::NodeState before, grid::NodeState after) {
        frames_.push_back({idx, before, after});
    }
    size_t size() const { return frames_.size(); }
    size_t cursor() const { return cursor_; }
    void setCursor(size_t c) { cursor_ = std::min(c, frames_.size()); }

    // Forward step (returns false if at end)
    bool advance(grid::Grid& g);
    // Reverse step (returns false if at start)
    bool reverse(grid::Grid& g);
    void rebuildTo(grid::Grid& g, size_t target);
private:
    std::vector<ReplayFrame> frames_;
    size_t cursor_ = 0;
};

} // namespace ronin::utilities
