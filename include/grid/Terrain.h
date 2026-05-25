#pragma once
#include <SFML/Graphics/Color.hpp>
#include <limits>

namespace ronin::grid {

enum class Terrain : unsigned char {
    Empty = 0,
    Grass,
    Sand,
    Water,
    Lava,
    Wall
};

inline float terrainCost(Terrain t) {
    switch (t) {
        case Terrain::Empty: return 1.f;
        case Terrain::Grass: return 1.f;
        case Terrain::Sand:  return 3.f;
        case Terrain::Water: return 6.f;
        case Terrain::Lava:  return 15.f;
        case Terrain::Wall:  return std::numeric_limits<float>::infinity();
    }
    return 1.f;
}

inline sf::Color terrainColor(Terrain t) {
    switch (t) {
        case Terrain::Empty: return {18, 22, 32};
        case Terrain::Grass: return {40, 70, 50};
        case Terrain::Sand:  return {180, 160, 90};
        case Terrain::Water: return {30, 90, 160};
        case Terrain::Lava:  return {180, 60, 30};
        case Terrain::Wall:  return {12, 12, 18};
    }
    return {0, 0, 0};
}

inline const char* terrainName(Terrain t) {
    switch (t) {
        case Terrain::Empty: return "Empty";
        case Terrain::Grass: return "Grass";
        case Terrain::Sand:  return "Sand";
        case Terrain::Water: return "Water";
        case Terrain::Lava:  return "Lava";
        case Terrain::Wall:  return "Wall";
    }
    return "?";
}

} // namespace ronin::grid
