#pragma once
#include "engine/Simulation.h"
#include "engine/Theme.h"

namespace ronin::ui {

class UiManager {
public:
    void draw(engine::Simulation& sim, engine::ThemeId& theme, grid::Terrain& brush, bool& showMinimap);
};

} // namespace ronin::ui
