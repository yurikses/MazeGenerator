#include "algorithms/AStarFinder.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <limits>
#include <queue>
#include <vector>

namespace {
constexpr Point noParent{-1, -1};

struct QueueNode {
    Point point;
    int gScore{0};
    int fScore{0};

    bool operator<(const QueueNode& other) const {
        if (fScore == other.fScore) {
            return gScore < other.gScore;
        }
        return fScore > other.fScore;
    }
};

int heuristic(Point left, Point right) {
    return std::abs(left.x - right.x) + std::abs(left.y - right.y);
}

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

AlgorithmResult AStarFinder::execute(Grid& maze, const Point& start, const Point& finish) {
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
    constexpr int infinity = std::numeric_limits<int>::max() / 4;
    std::vector<std::vector<int>> bestScore(height, std::vector<int>(width, infinity));
    std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));
    std::vector<std::vector<Point>> parent(height, std::vector<Point>(width, noParent));
    std::priority_queue<QueueNode> open;
    const std::array<Point, 4> directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    bestScore[start.y][start.x] = 0;
    open.push({start, 0, heuristic(start, finish)});

    bool found = false;
    while (!open.empty()) {
        const QueueNode node = open.top();
        open.pop();

        const Point current = node.point;
        if (closed[current.y][current.x]) {
            continue;
        }

        closed[current.y][current.x] = true;
        result.visited.push_back(current);
        notify({current, result.visited, {}, "A* search"});

        if (current == finish) {
            found = true;
            break;
        }

        for (const Point direction : directions) {
            const Point next{current.x + direction.x, current.y + direction.y};
            if (!isWalkable(maze, next) || closed[next.y][next.x]) {
                continue;
            }

            const int candidateScore = bestScore[current.y][current.x] + 1;
            if (candidateScore >= bestScore[next.y][next.x]) {
                continue;
            }

            parent[next.y][next.x] = current;
            bestScore[next.y][next.x] = candidateScore;
            open.push({next, candidateScore, candidateScore + heuristic(next, finish)});
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
    result.message = result.success ? "Path found with A*" : "Path not found";
    markResult(maze, result, start, finish);
    return result;
}

AlgorithmType AStarFinder::type() const {
    return AlgorithmType::AStarPathfinder;
}
