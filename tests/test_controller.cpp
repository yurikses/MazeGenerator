#include "controller/MazeController.h"

#include <gtest/gtest.h>

namespace {
class ControllerFixture : public testing::Test {
protected:
    MazeStorage storage{":memory:"};
    MazeController controller{storage};
};
} // namespace

TEST_F(ControllerFixture, StartsReadyWithEmptyGrid) {
    EXPECT_TRUE(controller.grid().empty());
    EXPECT_EQ(controller.statusMessage(), "Ready");
    EXPECT_TRUE(controller.canCreateMaze());
    EXPECT_TRUE(controller.canSaveMaze());
    EXPECT_FALSE(controller.canModerate());
}

TEST_F(ControllerFixture, GuestCannotGenerateOrSave) {
    controller.setAccessContext("guest", UserRole::Guest, false);

    EXPECT_FALSE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    EXPECT_FALSE(controller.saveCurrentMaze("guest maze", true));
    EXPECT_TRUE(controller.grid().empty());
}

TEST_F(ControllerFixture, BlockedUserCannotGenerate) {
    controller.setAccessContext("student", UserRole::User, true);

    EXPECT_FALSE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    EXPECT_FALSE(controller.canCreateMaze());
}

TEST_F(ControllerFixture, UserGeneratesDfsMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 21, 19));

    EXPECT_EQ(gridWidth(controller.grid()), 21);
    EXPECT_EQ(gridHeight(controller.grid()), 19);
    EXPECT_TRUE(isInside(controller.grid(), controller.start()));
    EXPECT_TRUE(isInside(controller.grid(), controller.finish()));
    EXPECT_TRUE(controller.lastResult().success);
}

TEST_F(ControllerFixture, UserGeneratesKruskalMazeAndSizeIsClamped) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::KruskalGenerator, 1, 1));

    EXPECT_EQ(gridWidth(controller.grid()), 5);
    EXPECT_EQ(gridHeight(controller.grid()), 5);
    EXPECT_TRUE(controller.lastResult().success);
}

TEST_F(ControllerFixture, RejectsPathfinderAsGenerator) {
    EXPECT_FALSE(controller.generateMaze(AlgorithmType::BFSPathfinder, 11, 11));
    EXPECT_TRUE(controller.grid().empty());
}

TEST_F(ControllerFixture, FindsPathAfterGeneration) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 21, 21));

    EXPECT_TRUE(controller.findPath(AlgorithmType::BFSPathfinder));
    EXPECT_TRUE(controller.lastResult().success);
    EXPECT_GT(controller.lastResult().metrics.pathLength, 0U);
}

TEST_F(ControllerFixture, RejectsGeneratorAsPathfinder) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));

    EXPECT_FALSE(controller.findPath(AlgorithmType::DFSGenerator));
}

TEST_F(ControllerFixture, RejectsPathSearchWithoutMaze) {
    EXPECT_FALSE(controller.findPath(AlgorithmType::BFSPathfinder));
}

TEST_F(ControllerFixture, SetStartRejectsWallCell) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));

    EXPECT_FALSE(controller.setStart({0, 0}));
}

TEST_F(ControllerFixture, SetStartAndFinishAcceptPassageCells) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    const Point oldStart = controller.start();
    const Point oldFinish = controller.finish();

    EXPECT_TRUE(controller.setStart(oldStart));
    EXPECT_TRUE(controller.setFinish(oldFinish));
    EXPECT_EQ(controller.start(), oldStart);
    EXPECT_EQ(controller.finish(), oldFinish);
}

TEST_F(ControllerFixture, ClearPathResetsSearchResult) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 21, 21));
    ASSERT_TRUE(controller.findPath(AlgorithmType::AStarPathfinder));
    ASSERT_GT(controller.lastResult().metrics.pathLength, 0U);

    controller.clearPath();

    EXPECT_EQ(controller.lastResult().metrics.pathLength, 0U);
    EXPECT_TRUE(controller.lastResult().path.empty());
}

TEST_F(ControllerFixture, SavesAndListsPublishedMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));

    EXPECT_TRUE(controller.saveCurrentMaze("public maze", true));
    const auto records = controller.listMazes();

    ASSERT_EQ(records.size(), 1U);
    EXPECT_EQ(records.front().name, "public maze");
}

TEST_F(ControllerFixture, NormalUserListHidesPrivateMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    ASSERT_TRUE(controller.saveCurrentMaze("private maze", false));

    EXPECT_TRUE(controller.listMazes().empty());
}

TEST_F(ControllerFixture, AdminListShowsPrivateMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    ASSERT_TRUE(controller.saveCurrentMaze("private maze", false));

    controller.setAccessContext("admin", UserRole::Admin, false);

    const auto records = controller.listMazes();
    ASSERT_EQ(records.size(), 1U);
    EXPECT_FALSE(records.front().published);
}

TEST_F(ControllerFixture, LoadsSavedMazeById) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    ASSERT_TRUE(controller.saveCurrentMaze("loadable", true));
    const int id = controller.listMazes().front().id;

    EXPECT_TRUE(controller.loadMaze(id));
    EXPECT_FALSE(controller.grid().empty());
    EXPECT_EQ(controller.statusMessage(), "Maze loaded: loadable");
}

TEST_F(ControllerFixture, RejectsMissingMazeLoad) {
    EXPECT_FALSE(controller.loadMaze(404));
}

TEST_F(ControllerFixture, AdminDeletesMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    ASSERT_TRUE(controller.saveCurrentMaze("delete me", true));
    const int id = controller.listMazes().front().id;

    controller.setAccessContext("admin", UserRole::Admin, false);

    EXPECT_TRUE(controller.deleteMaze(id));
    EXPECT_TRUE(controller.listMazes().empty());
}

TEST_F(ControllerFixture, NonAdminCannotDeleteMaze) {
    ASSERT_TRUE(controller.generateMaze(AlgorithmType::DFSGenerator, 11, 11));
    ASSERT_TRUE(controller.saveCurrentMaze("keep me", true));
    const int id = controller.listMazes().front().id;

    EXPECT_FALSE(controller.deleteMaze(id));
    EXPECT_EQ(controller.listMazes().size(), 1U);
}
