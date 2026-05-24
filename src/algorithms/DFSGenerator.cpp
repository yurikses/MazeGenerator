#include "algorithms/DFSGenerator.h"

#include <array>
#include <chrono>
#include <random>
#include <string>

namespace {
bool isCarvableCell(const Grid& maze, Point point) {
    return point.x > 0 && point.y > 0 && point.x < gridWidth(maze) - 1 && point.y < gridHeight(maze) - 1;
}

void fillWalls(Grid& maze) {
    for (auto& row : maze) {
        for (auto& cell : row) {
            cell = CellState::Wall;
        }
    }
}
} // namespace

AlgorithmResult DFSGenerator::execute(Grid& maze, const Point&, const Point&) {
    const auto startedAt = std::chrono::steady_clock::now();
    AlgorithmResult result;

    if (gridWidth(maze) < 5 || gridHeight(maze) < 5) {
        result.message = "Maze size must be at least 5x5";
        return result;
    }

    fillWalls(maze);

    std::mt19937 rng(std::random_device{}());
    std::vector<Point> stack;
    stack.push_back({1, 1});
    maze[1][1] = CellState::Passage;
    result.visited.push_back({1, 1});

    const std::array<Point, 4> directions{{{2, 0}, {-2, 0}, {0, 2}, {0, -2}}};

    while (!stack.empty()) {
        const Point current = stack.back();

        std::vector<Point> candidates;
        for (const Point direction : directions) {
            Point next{current.x + direction.x, current.y + direction.y};
            if (isCarvableCell(maze, next) && maze[next.y][next.x] == CellState::Wall) {
                candidates.push_back(next);
            }
        }

        notify({current, result.visited, {}, "DFS generation"});

        if (candidates.empty()) {
            stack.pop_back();
            continue;
        }

        std::uniform_int_distribution<std::size_t> distribution(0, candidates.size() - 1);
        const Point next = candidates[distribution(rng)];
        const Point between{(current.x + next.x) / 2, (current.y + next.y) / 2};
        maze[between.y][between.x] = CellState::Passage;
        maze[next.y][next.x] = CellState::Passage;
        result.visited.push_back(between);
        result.visited.push_back(next);
        stack.push_back(next);
    }

    const auto finishedAt = std::chrono::steady_clock::now();
    result.success = true;
    result.metrics.visitedCells = result.visited.size();
    result.metrics.elapsedMs = std::chrono::duration<double, std::milli>(finishedAt - startedAt).count();
    result.message = "Generated with DFS";
    return result;
}

AlgorithmType DFSGenerator::type() const {
    return AlgorithmType::DFSGenerator;
}
