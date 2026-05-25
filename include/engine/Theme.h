#pragma once
#include <SFML/Graphics/Color.hpp>
#include <imgui.h>

namespace ronin::engine {

enum class ThemeId { SamuraiNeon, Matrix, Tron, DarkMinimal };

struct Theme {
    sf::Color background;
    sf::Color accent;
    sf::Color glow;
    const char* name;
};

inline Theme makeTheme(ThemeId id) {
    switch (id) {
        case ThemeId::SamuraiNeon: return {{ 10, 10, 18 }, {220, 30, 80}, {255, 80, 130}, "Samurai Neon"};
        case ThemeId::Matrix:      return {{  4, 12,  6 }, { 40, 220, 90}, {120, 255, 130}, "Matrix"};
        case ThemeId::Tron:        return {{  6,  8, 18 }, { 60, 200, 255}, {120, 230, 255}, "Tron"};
        case ThemeId::DarkMinimal: return {{ 18, 18, 22 }, {200, 200, 210}, {255, 255, 255}, "Dark Minimal"};
    }
    return {{0,0,0},{255,255,255},{255,255,255},"?"};
}

void applyImGuiTheme(ThemeId id);

} // namespace ronin::engine
