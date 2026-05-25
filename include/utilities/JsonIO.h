#pragma once
#include "grid/Grid.h"
#include <string>

namespace ronin::utilities {

bool saveMap(const std::string& path, const grid::Grid& g);
bool loadMap(const std::string& path, grid::Grid& g);

} // namespace ronin::utilities
