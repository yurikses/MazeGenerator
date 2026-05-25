#include "ui/GUIMazeView.h"

#include <gtest/gtest.h>

namespace {
Grid grid(int width, int height) {
    return Grid(height, std::vector<CellState>(width, CellState::Passage));
}
} // namespace

TEST(GUIMazeView, CellAtReturnsNulloptForEmptyGrid) {
    GUIMazeView view;
    view.setViewport({{0.0F, 0.0F}, {100.0F, 100.0F}});

    EXPECT_FALSE(view.cellAt({10, 10}, {}).has_value());
}

TEST(GUIMazeView, CellAtMapsTopLeftCell) {
    GUIMazeView view;
    view.setViewport({{0.0F, 0.0F}, {100.0F, 100.0F}});

    const auto cell = view.cellAt({5, 5}, grid(10, 10));

    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(*cell, Point({0, 0}));
}

TEST(GUIMazeView, CellAtMapsBottomRightCell) {
    GUIMazeView view;
    view.setViewport({{0.0F, 0.0F}, {100.0F, 100.0F}});

    const auto cell = view.cellAt({95, 95}, grid(10, 10));

    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(*cell, Point({9, 9}));
}

TEST(GUIMazeView, CellAtRespectsViewportOffset) {
    GUIMazeView view;
    view.setViewport({{50.0F, 25.0F}, {100.0F, 100.0F}});

    const auto cell = view.cellAt({60, 35}, grid(10, 10));

    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(*cell, Point({1, 1}));
}

TEST(GUIMazeView, CellAtReturnsNulloptOutsideViewport) {
    GUIMazeView view;
    view.setViewport({{50.0F, 50.0F}, {100.0F, 100.0F}});

    EXPECT_FALSE(view.cellAt({49, 75}, grid(10, 10)).has_value());
    EXPECT_FALSE(view.cellAt({75, 49}, grid(10, 10)).has_value());
    EXPECT_FALSE(view.cellAt({151, 75}, grid(10, 10)).has_value());
    EXPECT_FALSE(view.cellAt({75, 151}, grid(10, 10)).has_value());
}

TEST(GUIMazeView, CellAtAccountsForCenteredNonSquareGrid) {
    GUIMazeView view;
    view.setViewport({{0.0F, 0.0F}, {100.0F, 100.0F}});

    const auto cell = view.cellAt({50, 50}, grid(5, 3));

    ASSERT_TRUE(cell.has_value());
    EXPECT_EQ(*cell, Point({2, 1}));
}
