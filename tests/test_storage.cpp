#include "core/MazeStorage.h"

#include <gtest/gtest.h>

namespace {
Grid sampleGrid() {
    return {{CellState::Wall, CellState::Passage, CellState::Start},
            {CellState::Visited, CellState::Path, CellState::Finish}};
}
} // namespace

TEST(MazeStorage, OpensInMemoryDatabase) {
    MazeStorage storage(":memory:");

    EXPECT_TRUE(storage.isOpen());
}

TEST(MazeStorage, RejectsEmptyGridSave) {
    MazeStorage storage(":memory:");

    EXPECT_FALSE(storage.saveMaze("empty", {}, "tester", true));
}

TEST(MazeStorage, SavesAndLoadsMazeWithMetadata) {
    MazeStorage storage(":memory:");

    ASSERT_TRUE(storage.saveMaze("sample", sampleGrid(), "tester", true));
    const auto records = storage.listMazes(true);
    ASSERT_EQ(records.size(), 1U);

    MazeRecord record;
    const auto loaded = storage.loadMaze(records.front().id, &record);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(record.name, "sample");
    EXPECT_EQ(record.owner, "tester");
    EXPECT_TRUE(record.published);
    EXPECT_EQ(gridWidth(*loaded), 3);
    EXPECT_EQ(gridHeight(*loaded), 2);
}

TEST(MazeStorage, ListMazesFiltersPrivateRecords) {
    MazeStorage storage(":memory:");

    ASSERT_TRUE(storage.saveMaze("public", sampleGrid(), "tester", true));
    ASSERT_TRUE(storage.saveMaze("private", sampleGrid(), "tester", false));

    EXPECT_EQ(storage.listMazes(false).size(), 1U);
    EXPECT_EQ(storage.listMazes(true).size(), 2U);
}

TEST(MazeStorage, LoadMissingMazeReturnsNullopt) {
    MazeStorage storage(":memory:");

    EXPECT_FALSE(storage.loadMaze(42).has_value());
}

TEST(MazeStorage, DeleteExistingMazeRemovesIt) {
    MazeStorage storage(":memory:");
    ASSERT_TRUE(storage.saveMaze("sample", sampleGrid(), "tester", true));
    const int id = storage.listMazes(true).front().id;

    EXPECT_TRUE(storage.deleteMaze(id));
    EXPECT_TRUE(storage.listMazes(true).empty());
}

TEST(MazeStorage, DeleteMissingMazeReturnsFalse) {
    MazeStorage storage(":memory:");

    EXPECT_FALSE(storage.deleteMaze(404));
}

TEST(MazeStorage, SerializeGridConvertsWallsAndWalkableCells) {
    const std::string serialized = MazeStorage::serializeGrid(sampleGrid());

    EXPECT_EQ(serialized, "#..\n...\n");
}

TEST(MazeStorage, DeserializeGridRestoresWallsAndPassages) {
    const Grid grid = MazeStorage::deserializeGrid("#.\n.#\n", 2, 2);

    ASSERT_EQ(gridHeight(grid), 2);
    ASSERT_EQ(gridWidth(grid), 2);
    EXPECT_EQ(grid[0][0], CellState::Wall);
    EXPECT_EQ(grid[0][1], CellState::Passage);
    EXPECT_EQ(grid[1][0], CellState::Passage);
    EXPECT_EQ(grid[1][1], CellState::Wall);
}

TEST(MazeStorage, DeserializeShortDataKeepsMissingCellsAsWalls) {
    const Grid grid = MazeStorage::deserializeGrid(".\n", 3, 2);

    EXPECT_EQ(grid[0][0], CellState::Passage);
    EXPECT_EQ(grid[0][1], CellState::Wall);
    EXPECT_EQ(grid[1][0], CellState::Wall);
}
