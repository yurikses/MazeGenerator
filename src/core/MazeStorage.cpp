#include "core/MazeStorage.h"

#include "core/AppLogger.h"

#include <sstream>

namespace {
const sqlite3_destructor_type sqliteTransient = SQLITE_TRANSIENT;

char serializeCell(CellState state) {
    return state == CellState::Wall ? '#' : '.';
}

CellState deserializeCell(char value) {
    return value == '#' ? CellState::Wall : CellState::Passage;
}

std::string textColumn(sqlite3_stmt* statement, int column) {
    const auto* value = sqlite3_column_text(statement, column);
    return value == nullptr ? std::string{} : reinterpret_cast<const char*>(value);
}
} // namespace

MazeStorage::MazeStorage(const std::string& databasePath) {
    if (sqlite3_open(databasePath.c_str(), &db_) != SQLITE_OK) {
        AppLogger::getInstance().log(LogLevel::ERROR, "Failed to open maze database");
        if (db_ != nullptr) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        return;
    }

    initializeSchema();
}

MazeStorage::~MazeStorage() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
    }
}

bool MazeStorage::isOpen() const {
    return db_ != nullptr;
}

void MazeStorage::initializeSchema() {
    if (!isOpen()) {
        return;
    }

    const char* sql =
        "CREATE TABLE IF NOT EXISTS mazes("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "width INTEGER NOT NULL,"
        "height INTEGER NOT NULL,"
        "data TEXT NOT NULL,"
        "owner TEXT NOT NULL,"
        "published INTEGER NOT NULL DEFAULT 1,"
        "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* error = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &error) != SQLITE_OK) {
        AppLogger::getInstance().log(LogLevel::ERROR, error == nullptr ? "Failed to initialize schema" : error);
        sqlite3_free(error);
    }
}

bool MazeStorage::saveMaze(const std::string& name, const Grid& grid, const std::string& owner, bool published) {
    if (!isOpen() || grid.empty()) {
        return false;
    }

    constexpr const char* sql =
        "INSERT INTO mazes(name, width, height, data, owner, published) VALUES(?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return false;
    }

    const std::string data = serializeGrid(grid);
    sqlite3_bind_text(statement, 1, name.c_str(), -1, sqliteTransient);
    sqlite3_bind_int(statement, 2, gridWidth(grid));
    sqlite3_bind_int(statement, 3, gridHeight(grid));
    sqlite3_bind_text(statement, 4, data.c_str(), -1, sqliteTransient);
    sqlite3_bind_text(statement, 5, owner.c_str(), -1, sqliteTransient);
    sqlite3_bind_int(statement, 6, published ? 1 : 0);

    const bool ok = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);
    return ok;
}

std::optional<Grid> MazeStorage::loadMaze(int id, MazeRecord* record) const {
    if (!isOpen()) {
        return std::nullopt;
    }

    constexpr const char* sql =
        "SELECT id, name, width, height, owner, published, created_at, data FROM mazes WHERE id = ?;";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_int(statement, 1, id);
    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    MazeRecord loadedRecord;
    loadedRecord.id = sqlite3_column_int(statement, 0);
    loadedRecord.name = textColumn(statement, 1);
    loadedRecord.width = sqlite3_column_int(statement, 2);
    loadedRecord.height = sqlite3_column_int(statement, 3);
    loadedRecord.owner = textColumn(statement, 4);
    loadedRecord.published = sqlite3_column_int(statement, 5) != 0;
    loadedRecord.createdAt = textColumn(statement, 6);
    const std::string data = textColumn(statement, 7);

    if (record != nullptr) {
        *record = loadedRecord;
    }

    sqlite3_finalize(statement);
    return deserializeGrid(data, loadedRecord.width, loadedRecord.height);
}

std::vector<MazeRecord> MazeStorage::listMazes(bool includePrivate) const {
    std::vector<MazeRecord> records;
    if (!isOpen()) {
        return records;
    }

    const char* sql = includePrivate
                          ? "SELECT id, name, width, height, owner, published, created_at FROM mazes ORDER BY id DESC;"
                          : "SELECT id, name, width, height, owner, published, created_at FROM mazes WHERE published = 1 ORDER BY id DESC;";

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return records;
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        MazeRecord record;
        record.id = sqlite3_column_int(statement, 0);
        record.name = textColumn(statement, 1);
        record.width = sqlite3_column_int(statement, 2);
        record.height = sqlite3_column_int(statement, 3);
        record.owner = textColumn(statement, 4);
        record.published = sqlite3_column_int(statement, 5) != 0;
        record.createdAt = textColumn(statement, 6);
        records.push_back(record);
    }

    sqlite3_finalize(statement);
    return records;
}

bool MazeStorage::deleteMaze(int id) {
    if (!isOpen()) {
        return false;
    }

    constexpr const char* sql = "DELETE FROM mazes WHERE id = ?;";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(statement, 1, id);
    const bool ok = sqlite3_step(statement) == SQLITE_DONE && sqlite3_changes(db_) > 0;
    sqlite3_finalize(statement);
    return ok;
}

std::string MazeStorage::serializeGrid(const Grid& grid) {
    std::ostringstream stream;
    for (const auto& row : grid) {
        for (CellState cell : row) {
            stream << serializeCell(cell);
        }
        stream << '\n';
    }
    return stream.str();
}

Grid MazeStorage::deserializeGrid(const std::string& data, int width, int height) {
    Grid grid(height, std::vector<CellState>(width, CellState::Wall));
    int x = 0;
    int y = 0;
    for (char value : data) {
        if (value == '\n') {
            x = 0;
            ++y;
            if (y >= height) {
                break;
            }
            continue;
        }
        if (x < width && y < height) {
            grid[y][x] = deserializeCell(value);
        }
        ++x;
    }
    return grid;
}
