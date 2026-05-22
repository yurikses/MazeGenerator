#pragma once

#include "core/types.h"

#include <optional>
#include <sqlite3.h>
#include <string>
#include <vector>

class MazeStorage {
public:
    explicit MazeStorage(const std::string& databasePath);
    ~MazeStorage();

    MazeStorage(const MazeStorage&) = delete;
    MazeStorage& operator=(const MazeStorage&) = delete;

    bool isOpen() const;
    bool saveMaze(const std::string& name, const Grid& grid, const std::string& owner, bool published);
    std::optional<Grid> loadMaze(int id, MazeRecord* record = nullptr) const;
    std::vector<MazeRecord> listMazes(bool includePrivate) const;
    bool deleteMaze(int id);

    static std::string serializeGrid(const Grid& grid);
    static Grid deserializeGrid(const std::string& data, int width, int height);

private:
    void initializeSchema();

    sqlite3* db_{nullptr};
};
