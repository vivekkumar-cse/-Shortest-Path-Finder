#include "engine/Application.h"
#include "utilities/Stats.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <cmath>

namespace ronin::engine {

Application::Application()
    : window_(sf::VideoMode(1600, 1000), "Ronin Pathfinder Engine", sf::Style::Default) {
    window_.setFramerateLimit(120);
    if (!ImGui::SFML::Init(window_)) {
        // Continue without ImGui? It's required for the UI.
    }
    applyImGuiTheme(theme_);
    camera_.setViewport(window_.getSize());
}

int Application::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        sf::Event e;
        while (window_.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(window_, e);
            handleEvent(e);
        }
        sf::Time dt = clock.restart();
        ImGui::SFML::Update(window_, dt);
        float dts = dt.asSeconds();
        update(dts);
        // Stats: FPS
        const_cast<utilities::Stats&>(sim_.stats()).fps =
            (dts > 0) ? 1.0 / dts : 0.0;

        ui_.draw(sim_, theme_, brush_, showMinimap_);
        render();
    }
    ImGui::SFML::Shutdown();
    return 0;
}

void Application::handleEvent(const sf::Event& e) {
    auto& io = ImGui::GetIO();
    if (e.type == sf::Event::Closed) { window_.close(); return; }
    if (e.type == sf::Event::Resized) {
        camera_.setViewport({e.size.width, e.size.height});
    }
    if (e.type == sf::Event::KeyPressed) {
        if (io.WantCaptureKeyboard) return;
        switch (e.key.code) {
            case sf::Keyboard::Space: sim_.isRunning() ? sim_.pause() : sim_.play(); break;
            case sf::Keyboard::R:     sim_.resetTraversal(); break;
            case sf::Keyboard::C:     sim_.clearAll(); break;
            case sf::Keyboard::M:     sim_.startMaze(0); break;
            case sf::Keyboard::S:     placingStart_ = true; placingEnd_ = false; break;
            case sf::Keyboard::E:     placingEnd_ = true; placingStart_ = false; break;
            case sf::Keyboard::Tab:
                sim_.mode = (sim_.mode == Mode::Grid) ? Mode::Graph : Mode::Grid;
                break;
            case sf::Keyboard::F11: {
                fullscreen_ = !fullscreen_;
                auto style = fullscreen_ ? sf::Style::Fullscreen : sf::Style::Default;
                auto mode = fullscreen_ ? sf::VideoMode::getDesktopMode()
                                        : sf::VideoMode(1600, 1000);
                window_.create(mode, "Ronin Pathfinder Engine", style);
                window_.setFramerateLimit(120);
                ImGui::SFML::UpdateFontTexture();
                camera_.setViewport(window_.getSize());
                break;
            }
            default: break;
        }
    }
    if (e.type == sf::Event::MouseWheelScrolled && !io.WantCaptureMouse) {
        float f = (e.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
        camera_.zoom(f, {});
    }
    if (e.type == sf::Event::MouseButtonPressed && !io.WantCaptureMouse) {
        if (e.mouseButton.button == sf::Mouse::Middle) {
            panning_ = true;
            lastMouse_ = {e.mouseButton.x, e.mouseButton.y};
        }
        if (sim_.mode == Mode::Grid) {
            bool left = e.mouseButton.button == sf::Mouse::Left;
            bool right = e.mouseButton.button == sf::Mouse::Right;
            if (left || right) {
                sf::Vector2f w = camera_.screenToWorld({e.mouseButton.x, e.mouseButton.y},
                                                       window_.getSize());
                paintAt(w, right);
            }
        } else { // Graph mode
            sf::Vector2f w = camera_.screenToWorld({e.mouseButton.x, e.mouseButton.y},
                                                   window_.getSize());
            if (e.mouseButton.button == sf::Mouse::Left) {
                int hit = sim_.graph().hitTest(w);
                if (hit < 0) {
                    sim_.graph().addNode(w);
                } else if (graphFromNode_ < 0) {
                    graphFromNode_ = hit;
                } else {
                    float dx = sim_.graph().nodes()[graphFromNode_].pos.x - sim_.graph().nodes()[hit].pos.x;
                    float dy = sim_.graph().nodes()[graphFromNode_].pos.y - sim_.graph().nodes()[hit].pos.y;
                    sim_.graph().addEdge(graphFromNode_, hit, std::sqrt(dx*dx+dy*dy));
                    graphFromNode_ = -1;
                }
            } else if (e.mouseButton.button == sf::Mouse::Right) {
                int hit = sim_.graph().hitTest(w);
                if (hit >= 0) {
                    if (placingStart_) { sim_.graph().start = hit; sim_.graph().nodes()[hit].state = graph::Node::Start; placingStart_ = false; }
                    else if (placingEnd_) { sim_.graph().end = hit; sim_.graph().nodes()[hit].state = graph::Node::End; placingEnd_ = false; }
                    if (sim_.graph().start >= 0 && sim_.graph().end >= 0) sim_.graph().runDijkstra();
                }
            }
        }
    }
    if (e.type == sf::Event::MouseButtonReleased) {
        if (e.mouseButton.button == sf::Mouse::Middle) panning_ = false;
    }
    if (e.type == sf::Event::MouseMoved) {
        if (panning_) {
            sf::Vector2i now{e.mouseMove.x, e.mouseMove.y};
            sf::Vector2i d = lastMouse_ - now;
            // pan proportionally to current view size
            sf::Vector2f vs = camera_.view().getSize();
            camera_.pan({d.x * vs.x / window_.getSize().x,
                         d.y * vs.y / window_.getSize().y});
            lastMouse_ = now;
        } else if (sim_.mode == Mode::Grid && !io.WantCaptureMouse) {
            // continuous paint while LMB / RMB held
            bool L = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            bool R = sf::Mouse::isButtonPressed(sf::Mouse::Right);
            if (L || R) {
                sf::Vector2f w = camera_.screenToWorld({e.mouseMove.x, e.mouseMove.y},
                                                       window_.getSize());
                paintAt(w, R);
            }
        }
    }
}

void Application::paintAt(sf::Vector2f world, bool erase) {
    auto& g = sim_.grid();
    int cx = static_cast<int>(world.x / g.cellSize());
    int cy = static_cast<int>(world.y / g.cellSize());
    if (!g.inBounds(cx, cy)) return;
    int idx = g.index(cx, cy);
    if (placingStart_) { g.setStart(idx); placingStart_ = false; sim_.resetTraversal(); return; }
    if (placingEnd_)   { g.setEnd(idx);   placingEnd_   = false; sim_.resetTraversal(); return; }
    auto& n = g.at(idx);
    if (n.state == grid::NodeState::Start || n.state == grid::NodeState::End) return;
    n.terrain = erase ? grid::Terrain::Empty : brush_;
}

void Application::update(float dt) {
    sim_.update(dt);
}

void Application::drawMinimap() {
    if (!showMinimap_) return;
    auto& g = sim_.grid();
    sf::View saved = window_.getView();
    sf::View ui(sf::FloatRect(0, 0, window_.getSize().x, window_.getSize().y));
    window_.setView(ui);

    float mmW = 220.f, mmH = mmW * g.height() / g.width();
    float mx = window_.getSize().x - mmW - 16.f, my = 16.f;
    sf::RectangleShape bg({mmW + 6, mmH + 6});
    bg.setPosition(mx - 3, my - 3);
    bg.setFillColor(sf::Color(0, 0, 0, 160));
    bg.setOutlineColor(sf::Color(120, 200, 255, 200));
    bg.setOutlineThickness(1.f);
    window_.draw(bg);
    sf::VertexArray quads(sf::Quads);
    float cw = mmW / g.width(), ch = mmH / g.height();
    quads.resize(g.size() * 4);
    for (int y = 0; y < g.height(); ++y)
        for (int x = 0; x < g.width(); ++x) {
            int i = g.index(x, y);
            const auto& n = g.at(i);
            sf::Color c = grid::terrainColor(n.terrain);
            if (n.state == grid::NodeState::Closed) c = {40, 90, 140};
            if (n.state == grid::NodeState::Path)   c = {255, 220, 80};
            if (n.state == grid::NodeState::Start)  c = {80, 255, 130};
            if (n.state == grid::NodeState::End)    c = {255, 90, 140};
            float x0 = mx + x * cw, y0 = my + y * ch;
            size_t q = static_cast<size_t>(i) * 4;
            quads[q+0] = sf::Vertex({x0, y0}, c);
            quads[q+1] = sf::Vertex({x0+cw, y0}, c);
            quads[q+2] = sf::Vertex({x0+cw, y0+ch}, c);
            quads[q+3] = sf::Vertex({x0, y0+ch}, c);
        }
    window_.draw(quads);
    window_.setView(saved);
}

void Application::render() {
    Theme th = makeTheme(theme_);
    window_.clear(th.background);

    camera_.apply(window_);
    if (sim_.mode == Mode::Grid) {
        window_.draw(sim_.grid());
        if (sim_.agentEnabled) sim_.agent().draw(window_, sim_.grid());
    } else {
        window_.draw(sim_.graph());
    }

    drawMinimap();

    ImGui::SFML::Render(window_);
    window_.display();
}

} // namespace ronin::engine
