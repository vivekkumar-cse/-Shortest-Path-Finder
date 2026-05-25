#pragma once
#include <SFML/Graphics.hpp>

namespace ronin::engine {

class Camera {
public:
    Camera() = default;
    void setViewport(sf::Vector2u winSize);
    void pan(sf::Vector2f delta);
    void zoom(float factor, sf::Vector2f at);
    void apply(sf::RenderTarget& t) const;
    const sf::View& view() const { return view_; }
    sf::Vector2f screenToWorld(sf::Vector2i p, sf::Vector2u winSize) const;
private:
    sf::View view_{sf::FloatRect(0, 0, 1280, 720)};
    float zoomLevel_ = 1.f;
};

} // namespace ronin::engine
