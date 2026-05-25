#pragma once
#include "engine/Camera.h"
#include "engine/Simulation.h"
#include "engine/Theme.h"
#include "ui/UiManager.h"
#include <SFML/Graphics.hpp>

namespace ronin::engine {

class Application {
public:
    Application();
    int run();

private:
    void handleEvent(const sf::Event& e);
    void update(float dt);
    void render();
    void paintAt(sf::Vector2f world, bool erase);
    void drawMinimap();

    sf::RenderWindow window_;
    Camera camera_;
    Simulation sim_;
    ui::UiManager ui_;
    ThemeId theme_ = ThemeId::SamuraiNeon;
    grid::Terrain brush_ = grid::Terrain::Wall;
    bool placingStart_ = false;
    bool placingEnd_ = false;
    bool panning_ = false;
    sf::Vector2i lastMouse_{};
    bool showMinimap_ = true;
    bool fullscreen_ = false;
    int graphFromNode_ = -1;
};

} // namespace ronin::engine
