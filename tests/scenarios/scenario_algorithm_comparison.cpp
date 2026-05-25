#include "algorithms/AStarFinder.h"
#include "algorithms/BFSFinder.h"
#include "algorithms/DFSGenerator.h"

#include <iostream>

namespace {
Point firstPassage(const Grid& grid) {
    for (int y = 0; y < gridHeight(grid); ++y) {
        for (int x = 0; x < gridWidth(grid); ++x) {
            Point point{x, y};
            if (isWalkable(grid, point)) {
                return point;
            }
        }
    }
    return {};
}

Point lastPassage(const Grid& grid) {
    for (int y = gridHeight(grid) - 1; y >= 0; --y) {
        for (int x = gridWidth(grid) - 1; x >= 0; --x) {
            Point point{x, y};
            if (isWalkable(grid, point)) {
                return point;
            }
        }
    }
    return {};
}
} // namespace

int main() {
    Grid maze(25, std::vector<CellState>(25, CellState::Wall));
    DFSGenerator generator;
    if (!generator.execute(maze, {}, {}).success) {
        std::cerr << "generation failed\n";
        return 1;
    }

    const Point start = firstPassage(maze);
    const Point finish = lastPassage(maze);
    Grid bfsMaze = maze;
    Grid aStarMaze = maze;
    BFSFinder bfs;
    AStarFinder aStar;

    const AlgorithmResult bfsResult = bfs.execute(bfsMaze, start, finish);
    const AlgorithmResult aStarResult = aStar.execute(aStarMaze, start, finish);

    if (!bfsResult.success || !aStarResult.success) {
        std::cerr << "search failed\n";
        return 2;
    }
    if (bfsResult.path.size() != aStarResult.path.size()) {
        std::cerr << "path length mismatch\n";
        return 3;
    }

    std::cout << "ScenarioAlgorithmComparison passed\n";
    return 0;
}
