#include "algorithms/AStarFinder.h"
#include "algorithms/BFSFinder.h"
#include "algorithms/DFSGenerator.h"
#include "algorithms/KruskalGenerator.h"

#include <gtest/gtest.h>

namespace {
Point firstPassage(const Grid& grid) {
    for (int y = 0; y < gridHeight(grid); ++y) {
        for (int x = 0; x < gridWidth(grid); ++x) {
            const Point point{x, y};
            if (isWalkable(grid, point)) {
                return point;
            }
        }
    }
    return {0, 0};
}

Point lastPassage(const Grid& grid) {
    for (int y = gridHeight(grid) - 1; y >= 0; --y) {
        for (int x = gridWidth(grid) - 1; x >= 0; --x) {
            const Point point{x, y};
            if (isWalkable(grid, point)) {
                return point;
            }
        }
    }
    return firstPassage(grid);
}
} // namespace

TEST(MazeAlgorithms, DfsGeneratorCreatesPassages) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    DFSGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.metrics.visitedCells, 0U);
    EXPECT_TRUE(isWalkable(grid, {1, 1}));
}

TEST(MazeAlgorithms, KruskalGeneratorCreatesPassages) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    KruskalGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.metrics.visitedCells, 0U);
    EXPECT_TRUE(isWalkable(grid, {1, 1}));
}

TEST(MazeAlgorithms, BfsFindsPathInGeneratedMaze) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    DFSGenerator generator;
    generator.execute(grid, {}, {});

    BFSFinder finder;
    const AlgorithmResult result = finder.execute(grid, firstPassage(grid), lastPassage(grid));

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.metrics.pathLength, 0U);
}

TEST(MazeAlgorithms, AStarFindsPathInGeneratedMaze) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    KruskalGenerator generator;
    generator.execute(grid, {}, {});

    AStarFinder finder;
    const AlgorithmResult result = finder.execute(grid, firstPassage(grid), lastPassage(grid));

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.metrics.pathLength, 0U);
}
