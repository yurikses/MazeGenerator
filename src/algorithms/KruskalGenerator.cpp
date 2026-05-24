#include "algorithms/KruskalGenerator.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace {
struct Edge {
    Point from;
    Point to;
    Point between;
};

class DisjointSet {
public:
    explicit DisjointSet(std::size_t size) : parent_(size), rank_(size, 0) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    bool unite(std::size_t left, std::size_t right) {
        left = find(left);
        right = find(right);
        if (left == right) {
            return false;
        }

        if (rank_[left] < rank_[right]) {
            std::swap(left, right);
        }
        parent_[right] = left;
        if (rank_[left] == rank_[right]) {
            ++rank_[left];
        }
        return true;
    }

private:
    std::size_t find(std::size_t value) {
        if (parent_[value] != value) {
            parent_[value] = find(parent_[value]);
        }
        return parent_[value];
    }

    std::vector<std::size_t> parent_;
    std::vector<int> rank_;
};

void fillWalls(Grid& maze) {
    for (auto& row : maze) {
        for (auto& cell : row) {
            cell = CellState::Wall;
        }
    }
}

bool isLogicalCell(const Grid& maze, Point point) {
    return point.x > 0 && point.y > 0 && point.x < gridWidth(maze) - 1 && point.y < gridHeight(maze) - 1;
}

std::size_t indexFor(const Grid& maze, Point point) {
    return static_cast<std::size_t>(point.y * gridWidth(maze) + point.x);
}
} // namespace

AlgorithmResult KruskalGenerator::execute(Grid& maze, const Point&, const Point&) {
    const auto startedAt = std::chrono::steady_clock::now();
    AlgorithmResult result;

    if (gridWidth(maze) < 5 || gridHeight(maze) < 5) {
        result.message = "Maze size must be at least 5x5";
        return result;
    }

    fillWalls(maze);

    std::vector<Edge> edges;
    for (int y = 1; y < gridHeight(maze) - 1; y += 2) {
        for (int x = 1; x < gridWidth(maze) - 1; x += 2) {
            maze[y][x] = CellState::Passage;
            const Point current{x, y};

            const Point right{x + 2, y};
            if (isLogicalCell(maze, right)) {
                edges.push_back({current, right, {x + 1, y}});
            }

            const Point down{x, y + 2};
            if (isLogicalCell(maze, down)) {
                edges.push_back({current, down, {x, y + 1}});
            }
        }
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(edges.begin(), edges.end(), rng);

    DisjointSet sets(static_cast<std::size_t>(gridWidth(maze) * gridHeight(maze)));
    for (const Edge& edge : edges) {
        if (!sets.unite(indexFor(maze, edge.from), indexFor(maze, edge.to))) {
            continue;
        }

        maze[edge.between.y][edge.between.x] = CellState::Passage;
        result.visited.push_back(edge.from);
        result.visited.push_back(edge.between);
        result.visited.push_back(edge.to);
        notify({edge.to, result.visited, {}, "Kruskal generation"});
    }

    const auto finishedAt = std::chrono::steady_clock::now();
    result.success = true;
    result.metrics.visitedCells = result.visited.size();
    result.metrics.elapsedMs = std::chrono::duration<double, std::milli>(finishedAt - startedAt).count();
    result.message = "Generated with Kruskal";
    return result;
}

AlgorithmType KruskalGenerator::type() const {
    return AlgorithmType::KruskalGenerator;
}
