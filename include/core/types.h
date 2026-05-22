#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class CellState {
    Wall,
    Passage,
    Visited,
    Frontier,
    Path,
    Start,
    Finish
};

enum class AlgorithmType {
    DFSGenerator,
    KruskalGenerator,
    BFSPathfinder,
    AStarPathfinder
};

enum class UserRole {
    Guest,
    User,
    Admin
};

enum class SelectionMode {
    Start,
    Finish
};

struct Point {
    int x{0};
    int y{0};

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

struct AlgorithmMetrics {
    std::size_t visitedCells{0};
    std::size_t pathLength{0};
    double elapsedMs{0.0};
};

struct VisualizationData {
    Point current{};
    std::vector<Point> visited;
    std::vector<Point> path;
    std::string message;
};

struct AlgorithmResult {
    bool success{false};
    std::vector<Point> visited;
    std::vector<Point> path;
    std::string message;
    AlgorithmMetrics metrics{};
};

struct MazeRecord {
    int id{0};
    std::string name;
    int width{0};
    int height{0};
    std::string owner;
    bool published{false};
    std::string createdAt;
};

struct UserAccount {
    std::string name;
    UserRole role{UserRole::Guest};
    bool blocked{false};
};

using Grid = std::vector<std::vector<CellState>>;

inline int gridHeight(const Grid& grid) {
    return static_cast<int>(grid.size());
}

inline int gridWidth(const Grid& grid) {
    return grid.empty() ? 0 : static_cast<int>(grid.front().size());
}

inline bool isInside(const Grid& grid, const Point& point) {
    return point.y >= 0 && point.y < gridHeight(grid) && point.x >= 0 && point.x < gridWidth(grid);
}

inline bool isWalkable(CellState state) {
    return state != CellState::Wall;
}

inline bool isWalkable(const Grid& grid, const Point& point) {
    return isInside(grid, point) && isWalkable(grid[point.y][point.x]);
}

inline bool isTransient(CellState state) {
    return state == CellState::Visited || state == CellState::Frontier ||
           state == CellState::Path || state == CellState::Start ||
           state == CellState::Finish;
}

inline void clearTransientStates(Grid& grid) {
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (isTransient(cell)) {
                cell = CellState::Passage;
            }
        }
    }
}

inline const char* algorithmName(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::DFSGenerator:
            return "DFS";
        case AlgorithmType::KruskalGenerator:
            return "Kruskal";
        case AlgorithmType::BFSPathfinder:
            return "BFS";
        case AlgorithmType::AStarPathfinder:
            return "A*";
    }
    return "Unknown";
}

inline const char* roleName(UserRole role) {
    switch (role) {
        case UserRole::Guest:
            return "Guest";
        case UserRole::User:
            return "User";
        case UserRole::Admin:
            return "Admin";
    }
    return "Unknown";
}
