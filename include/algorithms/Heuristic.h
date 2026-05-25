#pragma once
#include <cmath>
#include <algorithm>

namespace ronin::algorithms {

enum class Heuristic { Manhattan, Euclidean, Diagonal, Chebyshev };

inline float heuristic(Heuristic h, int x1, int y1, int x2, int y2) {
    float dx = static_cast<float>(std::abs(x1 - x2));
    float dy = static_cast<float>(std::abs(y1 - y2));
    switch (h) {
        case Heuristic::Manhattan: return dx + dy;
        case Heuristic::Euclidean: return std::sqrt(dx * dx + dy * dy);
        case Heuristic::Diagonal:  return (dx + dy) + (1.4142f - 2.f) * std::min(dx, dy);
        case Heuristic::Chebyshev: return std::max(dx, dy);
    }
    return 0.f;
}

inline const char* heuristicName(Heuristic h) {
    switch (h) {
        case Heuristic::Manhattan: return "Manhattan";
        case Heuristic::Euclidean: return "Euclidean";
        case Heuristic::Diagonal:  return "Diagonal";
        case Heuristic::Chebyshev: return "Chebyshev";
    }
    return "?";
}

} // namespace ronin::algorithms
