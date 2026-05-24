#pragma once

#include "core/types.h"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <optional>

class GUIMazeView {
public:
    void setViewport(sf::FloatRect viewport);
    void draw(sf::RenderWindow& window,
              const Grid& grid,
              Point start,
              bool hasStart,
              Point finish,
              bool hasFinish,
              const std::vector<Point>& visited,
              std::size_t visitedLimit,
              const std::vector<Point>& path,
              std::size_t pathLimit,
              bool revealGeneration) const;

    std::optional<Point> cellAt(sf::Vector2i pixel, const Grid& grid) const;

private:
    struct Layout {
        sf::Vector2f origin{};
        float cellSize{0.0F};
    };

    Layout layoutFor(const Grid& grid) const;
    static sf::Color baseColor(CellState state);

    sf::FloatRect viewport_{{360.0F, 24.0F}, {880.0F, 680.0F}};
};
