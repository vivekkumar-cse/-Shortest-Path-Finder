#include "engine/Camera.h"

namespace ronin::engine {

void Camera::setViewport(sf::Vector2u winSize) {
    view_.setSize(static_cast<float>(winSize.x), static_cast<float>(winSize.y));
}
void Camera::pan(sf::Vector2f delta) { view_.move(delta); }
void Camera::zoom(float factor, sf::Vector2f /*at*/) {
    zoomLevel_ *= factor;
    if (zoomLevel_ < 0.1f) { zoomLevel_ /= factor; return; }
    if (zoomLevel_ > 10.f) { zoomLevel_ /= factor; return; }
    view_.zoom(factor);
}
void Camera::apply(sf::RenderTarget& t) const { t.setView(view_); }
sf::Vector2f Camera::screenToWorld(sf::Vector2i p, sf::Vector2u winSize) const {
    sf::Vector2f norm{
        static_cast<float>(p.x) / static_cast<float>(winSize.x),
        static_cast<float>(p.y) / static_cast<float>(winSize.y)
    };
    sf::Vector2f size = view_.getSize();
    sf::Vector2f center = view_.getCenter();
    return { center.x - size.x * 0.5f + norm.x * size.x,
             center.y - size.y * 0.5f + norm.y * size.y };
}

} // namespace ronin::engine
