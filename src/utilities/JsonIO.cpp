#include "utilities/JsonIO.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace ronin::utilities {

bool saveMap(const std::string& path, const grid::Grid& g) {
    json j;
    j["width"] = g.width();
    j["height"] = g.height();
    j["start"] = g.start();
    j["end"] = g.end();
    json cells = json::array();
    for (const auto& n : g.data())
        cells.push_back(static_cast<int>(n.terrain));
    j["terrain"] = std::move(cells);
    std::ofstream o(path);
    if (!o) return false;
    o << j.dump(2);
    return true;
}

bool loadMap(const std::string& path, grid::Grid& g) {
    std::ifstream in(path);
    if (!in) return false;
    json j; in >> j;
    int w = j.value("width", g.width());
    int h = j.value("height", g.height());
    g.resize(w, h);
    if (j.contains("terrain")) {
        const auto& t = j["terrain"];
        for (size_t i = 0; i < t.size() && i < g.data().size(); ++i)
            g.at(static_cast<int>(i)).terrain = static_cast<grid::Terrain>(t[i].get<int>());
    }
    if (j.contains("start")) g.setStart(j["start"].get<int>());
    if (j.contains("end"))   g.setEnd(j["end"].get<int>());
    return true;
}

} // namespace ronin::utilities
