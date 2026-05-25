#pragma once
#include <chrono>

namespace ronin::utilities {

struct Stats {
    int    nodesVisited = 0;
    int    pathLength = 0;
    double execTimeMs = 0.0;
    double fps = 0.0;
    size_t memoryEstimate = 0;
    const char* algorithmName = "-";
    const char* complexity = "-";
};

inline const char* complexityFor(const char* algo) {
    using namespace std::string_view_literals;
    if (algo == nullptr) return "-";
    std::string_view s{algo};
    if (s == "Dijkstra")               return "O((V+E) log V)";
    if (s == "A*")                     return "O((V+E) log V)";
    if (s == "Greedy Best-First")      return "O(E)";
    if (s == "BFS")                    return "O(V+E)";
    if (s == "DFS")                    return "O(V+E)";
    if (s == "Bellman-Ford")           return "O(VE)";
    if (s == "Bidirectional BFS")      return "O(b^(d/2))";
    if (s == "Floyd-Warshall")         return "O(V^3)";
    if (s == "Jump Point Search")      return "O((V+E) log V)";
    return "-";
}

} // namespace ronin::utilities
