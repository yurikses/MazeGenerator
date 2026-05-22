#include "algorithms/BFSFinder.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <queue>
#include <vector>

namespace {
constexpr Point noParent{-1, -1};

std::vector<Point> reconstructPath(const std::vector<std::vector<Point>>& parent, Point start, Point finish) {
    std::vector<Point> path;
    for (Point current = finish; current != noParent; current = parent[current.y][current.x]) {
        path.push_back(current);
        if (current == start) {
            break;
        }
    }

    if (path.empty() || path.back() != start) {
        return {};
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void markResult(Grid& maze, const AlgorithmResult& result, Point start, Point finish) {
    for (Point point : result.visited) {
        if (point != start && point != finish && isInside(maze, point) && maze[point.y][point.x] != CellState::Wall) {
            maze[point.y][point.x] = CellState::Visited;
        }
    }

    for (Point point : result.path) {
        if (point != start && point != finish && isInside(maze, point) && maze[point.y][point.x] != CellState::Wall) {
            maze[point.y][point.x] = CellState::Path;
        }
    }

    if (isInside(maze, start)) {
        maze[start.y][start.x] = CellState::Start;
    }
    if (isInside(maze, finish)) {
        maze[finish.y][finish.x] = CellState::Finish;
    }
}
} // namespace

AlgorithmResult BFSFinder::execute(Grid& maze, const Point& start, const Point& finish) {
    const auto startedAt = std::chrono::steady_clock::now();
    AlgorithmResult result;
    clearTransientStates(maze);

    if (!isWalkable(maze, start) || !isWalkable(maze, finish)) {
        result.message = "Start and finish must be placed on passages";
        markResult(maze, result, start, finish);
        return result;
    }

    const int width = gridWidth(maze);
    const int height = gridHeight(maze);
    std::vector<std::vector<bool>> seen(height, std::vector<bool>(width, false));
    std::vector<std::vector<Point>> parent(height, std::vector<Point>(width, noParent));
    std::queue<Point> queue;
    const std::array<Point, 4> directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    queue.push(start);
    seen[start.y][start.x] = true;

    bool found = false;
    while (!queue.empty()) {
        const Point current = queue.front();
        queue.pop();
        result.visited.push_back(current);
        notify({current, result.visited, {}, "BFS search"});

        if (current == finish) {
            found = true;
            break;
        }

        for (const Point direction : directions) {
            const Point next{current.x + direction.x, current.y + direction.y};
            if (!isWalkable(maze, next) || seen[next.y][next.x]) {
                continue;
            }

            seen[next.y][next.x] = true;
            parent[next.y][next.x] = current;
            queue.push(next);
        }
    }

    if (found) {
        result.path = reconstructPath(parent, start, finish);
        result.success = !result.path.empty();
    }

    const auto finishedAt = std::chrono::steady_clock::now();
    result.metrics.visitedCells = result.visited.size();
    result.metrics.pathLength = result.path.size();
    result.metrics.elapsedMs = std::chrono::duration<double, std::milli>(finishedAt - startedAt).count();
    result.message = result.success ? "Path found with BFS" : "Path not found";
    markResult(maze, result, start, finish);
    return result;
}

AlgorithmType BFSFinder::type() const {
    return AlgorithmType::BFSPathfinder;
}
