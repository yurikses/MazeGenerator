#include "core/types.h"

#include <gtest/gtest.h>

TEST(CoreTypes, PointEqualityComparesCoordinates) {
    EXPECT_EQ(Point({1, 2}), Point({1, 2}));
    EXPECT_NE(Point({1, 2}), Point({2, 1}));
}

TEST(CoreTypes, GridDimensionsHandleEmptyAndFilledGrids) {
    EXPECT_EQ(gridWidth({}), 0);
    EXPECT_EQ(gridHeight({}), 0);

    const Grid grid(3, std::vector<CellState>(4, CellState::Wall));
    EXPECT_EQ(gridWidth(grid), 4);
    EXPECT_EQ(gridHeight(grid), 3);
}

TEST(CoreTypes, IsInsideChecksBoundaries) {
    const Grid grid(2, std::vector<CellState>(3, CellState::Wall));

    EXPECT_TRUE(isInside(grid, {0, 0}));
    EXPECT_TRUE(isInside(grid, {2, 1}));
    EXPECT_FALSE(isInside(grid, {-1, 0}));
    EXPECT_FALSE(isInside(grid, {0, -1}));
    EXPECT_FALSE(isInside(grid, {3, 0}));
    EXPECT_FALSE(isInside(grid, {0, 2}));
}

TEST(CoreTypes, WalkableCellsExcludeOnlyWalls) {
    EXPECT_FALSE(isWalkable(CellState::Wall));
    EXPECT_TRUE(isWalkable(CellState::Passage));
    EXPECT_TRUE(isWalkable(CellState::Visited));
    EXPECT_TRUE(isWalkable(CellState::Path));
    EXPECT_TRUE(isWalkable(CellState::Start));
    EXPECT_TRUE(isWalkable(CellState::Finish));
}

TEST(CoreTypes, GridWalkabilityRequiresInsidePoint) {
    Grid grid(2, std::vector<CellState>(2, CellState::Passage));
    grid[1][1] = CellState::Wall;

    EXPECT_TRUE(isWalkable(grid, {0, 0}));
    EXPECT_FALSE(isWalkable(grid, {1, 1}));
    EXPECT_FALSE(isWalkable(grid, {5, 5}));
}

TEST(CoreTypes, TransientStatesAreDetected) {
    EXPECT_FALSE(isTransient(CellState::Wall));
    EXPECT_FALSE(isTransient(CellState::Passage));
    EXPECT_TRUE(isTransient(CellState::Visited));
    EXPECT_TRUE(isTransient(CellState::Frontier));
    EXPECT_TRUE(isTransient(CellState::Path));
    EXPECT_TRUE(isTransient(CellState::Start));
    EXPECT_TRUE(isTransient(CellState::Finish));
}

TEST(CoreTypes, ClearTransientStatesKeepsWallsAndPassages) {
    Grid grid{{CellState::Wall, CellState::Passage, CellState::Visited},
              {CellState::Path, CellState::Start, CellState::Finish}};

    clearTransientStates(grid);

    EXPECT_EQ(grid[0][0], CellState::Wall);
    EXPECT_EQ(grid[0][1], CellState::Passage);
    EXPECT_EQ(grid[0][2], CellState::Passage);
    EXPECT_EQ(grid[1][0], CellState::Passage);
    EXPECT_EQ(grid[1][1], CellState::Passage);
    EXPECT_EQ(grid[1][2], CellState::Passage);
}

TEST(CoreTypes, AlgorithmNamesAreStable) {
    EXPECT_STREQ(algorithmName(AlgorithmType::DFSGenerator), "DFS");
    EXPECT_STREQ(algorithmName(AlgorithmType::KruskalGenerator), "Kruskal");
    EXPECT_STREQ(algorithmName(AlgorithmType::BFSPathfinder), "BFS");
    EXPECT_STREQ(algorithmName(AlgorithmType::AStarPathfinder), "A*");
}

TEST(CoreTypes, RoleNamesAreStable) {
    EXPECT_STREQ(roleName(UserRole::Guest), "Guest");
    EXPECT_STREQ(roleName(UserRole::User), "User");
    EXPECT_STREQ(roleName(UserRole::Admin), "Admin");
}
