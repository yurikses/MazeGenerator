#include "algorithms/AStarFinder.h"
#include "algorithms/BFSFinder.h"
#include "algorithms/DFSGenerator.h"
#include "algorithms/KruskalGenerator.h"

#include <gtest/gtest.h>

#include <queue>

namespace {
Grid openGrid(int width, int height) {
    return Grid(height, std::vector<CellState>(width, CellState::Passage));
}

Grid isolatedEndpointsGrid() {
    Grid grid(3, std::vector<CellState>(3, CellState::Wall));
    grid[0][0] = CellState::Passage;
    grid[2][2] = CellState::Passage;
    return grid;
}

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

std::size_t countWalkable(const Grid& grid) {
    std::size_t count = 0;
    for (const auto& row : grid) {
        for (CellState cell : row) {
            if (isWalkable(cell)) {
                ++count;
            }
        }
    }
    return count;
}

std::size_t reachableWalkableCount(const Grid& grid, Point start) {
    std::vector<std::vector<bool>> seen(static_cast<std::size_t>(gridHeight(grid)),
                                        std::vector<bool>(static_cast<std::size_t>(gridWidth(grid)), false));
    std::queue<Point> queue;
    queue.push(start);
    seen[static_cast<std::size_t>(start.y)][static_cast<std::size_t>(start.x)] = true;

    std::size_t count = 0;
    const Point directions[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    while (!queue.empty()) {
        const Point current = queue.front();
        queue.pop();
        ++count;

        for (Point direction : directions) {
            const Point next{current.x + direction.x, current.y + direction.y};
            if (isWalkable(grid, next) && !seen[static_cast<std::size_t>(next.y)][static_cast<std::size_t>(next.x)]) {
                seen[static_cast<std::size_t>(next.y)][static_cast<std::size_t>(next.x)] = true;
                queue.push(next);
            }
        }
    }
    return count;
}

void expectGeneratedMazeIsConnected(Grid grid, IMazeAlgorithm& generator) {
    const AlgorithmResult result = generator.execute(grid, {}, {});
    ASSERT_TRUE(result.success);
    const Point start = firstPassage(grid);
    EXPECT_EQ(reachableWalkableCount(grid, start), countWalkable(grid));
}
} // namespace

TEST(DFSGenerator, RejectsTooSmallMaze) {
    Grid grid(4, std::vector<CellState>(4, CellState::Wall));
    DFSGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_FALSE(result.success);
    EXPECT_EQ(countWalkable(grid), 0U);
}

TEST(DFSGenerator, GeneratesMinimumMaze) {
    Grid grid(5, std::vector<CellState>(5, CellState::Wall));
    DFSGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(countWalkable(grid), 1U);
    EXPECT_EQ(generator.type(), AlgorithmType::DFSGenerator);
}

TEST(DFSGenerator, KeepsRequestedDimensions) {
    Grid grid(17, std::vector<CellState>(23, CellState::Wall));
    DFSGenerator generator;

    generator.execute(grid, {}, {});

    EXPECT_EQ(gridWidth(grid), 23);
    EXPECT_EQ(gridHeight(grid), 17);
}

TEST(DFSGenerator, ProducesConnectedMaze) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    DFSGenerator generator;

    expectGeneratedMazeIsConnected(grid, generator);
}

TEST(DFSGenerator, ReportsVisitedCarvingSteps) {
    Grid grid(15, std::vector<CellState>(15, CellState::Wall));
    DFSGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.visited.size(), 1U);
    EXPECT_EQ(result.metrics.visitedCells, result.visited.size());
}

TEST(KruskalGenerator, RejectsTooSmallMaze) {
    Grid grid(4, std::vector<CellState>(4, CellState::Wall));
    KruskalGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_FALSE(result.success);
    EXPECT_EQ(countWalkable(grid), 0U);
}

TEST(KruskalGenerator, GeneratesMinimumMaze) {
    Grid grid(5, std::vector<CellState>(5, CellState::Wall));
    KruskalGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(countWalkable(grid), 1U);
    EXPECT_EQ(generator.type(), AlgorithmType::KruskalGenerator);
}

TEST(KruskalGenerator, KeepsRequestedDimensions) {
    Grid grid(19, std::vector<CellState>(25, CellState::Wall));
    KruskalGenerator generator;

    generator.execute(grid, {}, {});

    EXPECT_EQ(gridWidth(grid), 25);
    EXPECT_EQ(gridHeight(grid), 19);
}

TEST(KruskalGenerator, ProducesConnectedMaze) {
    Grid grid(23, std::vector<CellState>(23, CellState::Wall));
    KruskalGenerator generator;

    expectGeneratedMazeIsConnected(grid, generator);
}

TEST(KruskalGenerator, ReportsVisitedCarvingSteps) {
    Grid grid(15, std::vector<CellState>(15, CellState::Wall));
    KruskalGenerator generator;

    const AlgorithmResult result = generator.execute(grid, {}, {});

    EXPECT_TRUE(result.success);
    EXPECT_GT(result.visited.size(), 1U);
    EXPECT_EQ(result.metrics.visitedCells, result.visited.size());
}

TEST(BFSFinder, FindsShortestPathOnOpenGrid) {
    Grid grid = openGrid(5, 5);
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {4, 4});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.path.size(), 9U);
    EXPECT_EQ(result.metrics.pathLength, 9U);
}

TEST(BFSFinder, ReturnsSingleCellPathWhenStartEqualsFinish) {
    Grid grid = openGrid(3, 3);
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {1, 1}, {1, 1});

    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.path.size(), 1U);
    EXPECT_EQ(result.path.front(), Point({1, 1}));
}

TEST(BFSFinder, RejectsWallEndpoint) {
    Grid grid = openGrid(3, 3);
    grid[0][0] = CellState::Wall;
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 2});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.path.empty());
}

TEST(BFSFinder, ReportsNoPathForIsolatedEndpoints) {
    Grid grid = isolatedEndpointsGrid();
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 2});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.path.empty());
    EXPECT_GT(result.metrics.visitedCells, 0U);
}

TEST(BFSFinder, MarksVisitedPathAndEndpoints) {
    Grid grid = openGrid(3, 1);
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 0});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(grid[0][0], CellState::Start);
    EXPECT_EQ(grid[0][1], CellState::Path);
    EXPECT_EQ(grid[0][2], CellState::Finish);
}

TEST(BFSFinder, ClearsPreviousTransientStatesBeforeSearch) {
    Grid grid = openGrid(3, 1);
    grid[0][1] = CellState::Visited;
    BFSFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 0});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(grid[0][1], CellState::Path);
}

TEST(AStarFinder, FindsShortestPathOnOpenGrid) {
    Grid grid = openGrid(5, 5);
    AStarFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {4, 4});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.path.size(), 9U);
    EXPECT_EQ(result.metrics.pathLength, 9U);
}

TEST(AStarFinder, ReturnsSingleCellPathWhenStartEqualsFinish) {
    Grid grid = openGrid(3, 3);
    AStarFinder finder;

    const AlgorithmResult result = finder.execute(grid, {1, 1}, {1, 1});

    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.path.size(), 1U);
    EXPECT_EQ(result.path.front(), Point({1, 1}));
}

TEST(AStarFinder, RejectsWallEndpoint) {
    Grid grid = openGrid(3, 3);
    grid[2][2] = CellState::Wall;
    AStarFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 2});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.path.empty());
}

TEST(AStarFinder, ReportsNoPathForIsolatedEndpoints) {
    Grid grid = isolatedEndpointsGrid();
    AStarFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 2});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.path.empty());
    EXPECT_GT(result.metrics.visitedCells, 0U);
}

TEST(AStarFinder, MarksVisitedPathAndEndpoints) {
    Grid grid = openGrid(3, 1);
    AStarFinder finder;

    const AlgorithmResult result = finder.execute(grid, {0, 0}, {2, 0});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(grid[0][0], CellState::Start);
    EXPECT_EQ(grid[0][1], CellState::Path);
    EXPECT_EQ(grid[0][2], CellState::Finish);
}

TEST(AStarFinder, MatchesBfsPathLengthOnGeneratedMaze) {
    Grid grid(21, std::vector<CellState>(21, CellState::Wall));
    DFSGenerator generator;
    generator.execute(grid, {}, {});

    Grid bfsGrid = grid;
    Grid aStarGrid = grid;
    const Point start = firstPassage(grid);
    const Point finish = lastPassage(grid);
    BFSFinder bfs;
    AStarFinder aStar;

    const AlgorithmResult bfsResult = bfs.execute(bfsGrid, start, finish);
    const AlgorithmResult aStarResult = aStar.execute(aStarGrid, start, finish);

    ASSERT_TRUE(bfsResult.success);
    ASSERT_TRUE(aStarResult.success);
    EXPECT_EQ(aStarResult.path.size(), bfsResult.path.size());
}
