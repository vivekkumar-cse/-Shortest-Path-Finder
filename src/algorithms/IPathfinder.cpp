#include "algorithms/IPathfinder.h"

namespace ronin::algorithms {

void IPathfinder::reconstruct(grid::Grid& g) {
    path_.clear();
    if (!succeeded_) return;
    int cur = g.end();
    while (cur != -1 && cur != g.start()) {
        path_.push_back(cur);
        cur = g.at(cur).parent;
    }
    if (cur == g.start()) path_.push_back(cur);
    pathLength_ = static_cast<int>(path_.size());
    for (int i : path_) {
        auto& n = g.at(i);
        if (n.state != grid::NodeState::Start && n.state != grid::NodeState::End)
            n.state = grid::NodeState::Path;
    }
}

} // namespace ronin::algorithms
