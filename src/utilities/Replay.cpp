#include "utilities/Replay.h"

namespace ronin::utilities {

bool Replay::advance(grid::Grid& g) {
    if (cursor_ >= frames_.size()) return false;
    auto& f = frames_[cursor_++];
    auto& n = g.at(f.nodeIndex);
    if (n.state != grid::NodeState::Start && n.state != grid::NodeState::End)
        n.state = f.after;
    return true;
}
bool Replay::reverse(grid::Grid& g) {
    if (cursor_ == 0) return false;
    auto& f = frames_[--cursor_];
    auto& n = g.at(f.nodeIndex);
    if (n.state != grid::NodeState::Start && n.state != grid::NodeState::End)
        n.state = f.before;
    return true;
}
void Replay::rebuildTo(grid::Grid& g, size_t target) {
    while (cursor_ < target && advance(g)) {}
    while (cursor_ > target && reverse(g)) {}
}

} // namespace ronin::utilities
