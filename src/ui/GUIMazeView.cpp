#include "ui/GUIMazeView.h"

#include <algorithm>
#include <cmath>

void GUIMazeView::setViewport(sf::FloatRect viewport) {
    viewport_ = viewport;
}

void GUIMazeView::draw(sf::RenderWindow& window,
                       const Grid& grid,
                       Point start,
                       Point finish,
                       const std::vector<Point>& visited,
                       std::size_t visitedLimit,
                       const std::vector<Point>& path,
                       std::size_t pathLimit) const {
    if (grid.empty()) {
        return;
    }

    const Layout layout = layoutFor(grid);
    if (layout.cellSize <= 0.0F) {
        return;
    }

    sf::RectangleShape cell;
    cell.setSize({std::max(1.0F, layout.cellSize - 1.0F), std::max(1.0F, layout.cellSize - 1.0F)});

    sf::RectangleShape panel;
    panel.setPosition(viewport_.position);
    panel.setSize(viewport_.size);
    panel.setFillColor(sf::Color(18, 23, 31));
    window.draw(panel);

    for (int y = 0; y < gridHeight(grid); ++y) {
        for (int x = 0; x < gridWidth(grid); ++x) {
            CellState state = grid[y][x];
            if (state == CellState::Visited || state == CellState::Path) {
                state = CellState::Passage;
            }

            cell.setPosition({layout.origin.x + static_cast<float>(x) * layout.cellSize,
                              layout.origin.y + static_cast<float>(y) * layout.cellSize});
            cell.setFillColor(baseColor(state));
            window.draw(cell);
        }
    }

    const auto drawOverlay = [&](Point point, sf::Color color) {
        if (!isInside(grid, point)) {
            return;
        }
        cell.setPosition({layout.origin.x + static_cast<float>(point.x) * layout.cellSize,
                          layout.origin.y + static_cast<float>(point.y) * layout.cellSize});
        cell.setFillColor(color);
        window.draw(cell);
    };

    const std::size_t visibleVisited = std::min(visitedLimit, visited.size());
    for (std::size_t index = 0; index < visibleVisited; ++index) {
        drawOverlay(visited[index], baseColor(CellState::Visited));
    }

    const std::size_t visiblePath = std::min(pathLimit, path.size());
    for (std::size_t index = 0; index < visiblePath; ++index) {
        drawOverlay(path[index], baseColor(CellState::Path));
    }

    drawOverlay(start, baseColor(CellState::Start));
    drawOverlay(finish, baseColor(CellState::Finish));
}

std::optional<Point> GUIMazeView::cellAt(sf::Vector2i pixel, const Grid& grid) const {
    if (grid.empty()) {
        return std::nullopt;
    }

    const Layout layout = layoutFor(grid);
    if (layout.cellSize <= 0.0F) {
        return std::nullopt;
    }

    const float px = static_cast<float>(pixel.x);
    const float py = static_cast<float>(pixel.y);
    const float left = layout.origin.x;
    const float top = layout.origin.y;
    const float right = left + static_cast<float>(gridWidth(grid)) * layout.cellSize;
    const float bottom = top + static_cast<float>(gridHeight(grid)) * layout.cellSize;

    if (px < left || py < top || px >= right || py >= bottom) {
        return std::nullopt;
    }

    return Point{static_cast<int>((px - left) / layout.cellSize), static_cast<int>((py - top) / layout.cellSize)};
}

GUIMazeView::Layout GUIMazeView::layoutFor(const Grid& grid) const {
    const float horizontalCell = viewport_.size.x / static_cast<float>(std::max(1, gridWidth(grid)));
    const float verticalCell = viewport_.size.y / static_cast<float>(std::max(1, gridHeight(grid)));
    const float cellSize = std::floor(std::min(horizontalCell, verticalCell));

    const sf::Vector2f mazeSize{cellSize * static_cast<float>(gridWidth(grid)),
                                cellSize * static_cast<float>(gridHeight(grid))};
    const sf::Vector2f origin{viewport_.position.x + (viewport_.size.x - mazeSize.x) * 0.5F,
                              viewport_.position.y + (viewport_.size.y - mazeSize.y) * 0.5F};

    return {origin, cellSize};
}

sf::Color GUIMazeView::baseColor(CellState state) {
    switch (state) {
        case CellState::Wall:
            return sf::Color(29, 35, 45);
        case CellState::Passage:
            return sf::Color(232, 236, 241);
        case CellState::Visited:
            return sf::Color(92, 170, 255);
        case CellState::Frontier:
            return sf::Color(250, 197, 77);
        case CellState::Path:
            return sf::Color(47, 191, 113);
        case CellState::Start:
            return sf::Color(49, 130, 206);
        case CellState::Finish:
            return sf::Color(225, 83, 83);
    }
    return sf::Color::Magenta;
}
