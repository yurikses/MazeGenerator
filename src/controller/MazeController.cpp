#include "controller/MazeController.h"

#include "core/AppLogger.h"
#include "core/UserManager.h"
#include "patterns/factory.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace {
Grid makeEmptyGrid(int width, int height) {
    width = std::clamp(width, 5, 101);
    height = std::clamp(height, 5, 101);
    return Grid(height, std::vector<CellState>(width, CellState::Wall));
}

Grid saveableGrid(Grid grid) {
    clearTransientStates(grid);
    return grid;
}
} // namespace

MazeController::MazeController(MazeStorage& storage) : storage_(storage) {
    generateMaze(AlgorithmType::DFSGenerator, 31, 31);
}

void MazeController::setAccessContext(std::string userName, UserRole role, bool blocked) {
    userName_ = std::move(userName);
    role_ = role;
    blocked_ = blocked;
}

bool MazeController::generateMaze(AlgorithmType generatorType, int width, int height) {
    if (!canCreateMaze()) {
        setStatus("Current role cannot create mazes");
        return false;
    }
    if (!AlgorithmFactory::isGenerator(generatorType)) {
        setStatus("Selected algorithm is not a generator");
        return false;
    }

    grid_ = makeEmptyGrid(width, height);
    auto generator = AlgorithmFactory::create(generatorType);
    lastResult_ = generator->execute(grid_, {}, {});
    if (!lastResult_.success) {
        setStatus(lastResult_.message);
        return false;
    }

    start_ = firstPassage();
    finish_ = lastPassage();
    overlayEndpoints();

    std::ostringstream message;
    message << lastResult_.message << ": " << gridWidth(grid_) << "x" << gridHeight(grid_);
    setStatus(message.str());
    AppLogger::getInstance().log(LogLevel::INFO, statusMessage_);
    return true;
}

bool MazeController::findPath(AlgorithmType pathfinderType) {
    if (blocked_) {
        setStatus("Blocked user cannot run actions");
        return false;
    }
    if (!AlgorithmFactory::isPathfinder(pathfinderType)) {
        setStatus("Selected algorithm is not a pathfinder");
        return false;
    }
    if (grid_.empty()) {
        setStatus("Generate or load a maze first");
        return false;
    }

    clearPath();
    auto pathfinder = AlgorithmFactory::create(pathfinderType);
    lastResult_ = pathfinder->execute(grid_, start_, finish_);
    setStatus(lastResult_.message);

    std::ostringstream logMessage;
    logMessage << lastResult_.message << ", visited=" << lastResult_.metrics.visitedCells
               << ", path=" << lastResult_.metrics.pathLength;
    AppLogger::getInstance().log(lastResult_.success ? LogLevel::INFO : LogLevel::WARN, logMessage.str());
    return lastResult_.success;
}

bool MazeController::setStart(Point point) {
    if (!isValidEndpoint(point)) {
        setStatus("Start must be placed on a passage");
        return false;
    }

    clearTransientStates(grid_);
    start_ = point;
    lastResult_ = {};
    overlayEndpoints();
    setStatus("Start point updated");
    return true;
}

bool MazeController::setFinish(Point point) {
    if (!isValidEndpoint(point)) {
        setStatus("Finish must be placed on a passage");
        return false;
    }

    clearTransientStates(grid_);
    finish_ = point;
    lastResult_ = {};
    overlayEndpoints();
    setStatus("Finish point updated");
    return true;
}

void MazeController::clearPath() {
    clearTransientStates(grid_);
    overlayEndpoints();
    lastResult_ = {};
}

bool MazeController::saveCurrentMaze(const std::string& name, bool published) {
    if (!canSaveMaze()) {
        setStatus("Current role cannot save mazes");
        return false;
    }
    if (grid_.empty()) {
        setStatus("Generate a maze before saving");
        return false;
    }

    const std::string finalName = name.empty() ? "Maze" : name;
    const bool saved = storage_.saveMaze(finalName, saveableGrid(grid_), userName_, published);
    setStatus(saved ? "Maze saved" : "Failed to save maze");
    AppLogger::getInstance().log(saved ? LogLevel::INFO : LogLevel::ERROR, statusMessage_);
    return saved;
}

bool MazeController::loadMaze(int id) {
    MazeRecord record;
    auto loaded = storage_.loadMaze(id, &record);
    if (!loaded.has_value()) {
        setStatus("Maze was not found");
        return false;
    }
    if (!record.published && !canModerate() && record.owner != userName_) {
        setStatus("Current role cannot load private maze");
        return false;
    }

    grid_ = std::move(*loaded);
    clearTransientStates(grid_);
    start_ = firstPassage();
    finish_ = lastPassage();
    lastResult_ = {};
    overlayEndpoints();

    setStatus("Maze loaded: " + record.name);
    AppLogger::getInstance().log(LogLevel::INFO, statusMessage_);
    return true;
}

bool MazeController::deleteMaze(int id) {
    if (!canModerate()) {
        setStatus("Only administrator can delete mazes");
        return false;
    }

    const bool deleted = storage_.deleteMaze(id);
    setStatus(deleted ? "Maze deleted" : "Failed to delete maze");
    AppLogger::getInstance().log(deleted ? LogLevel::INFO : LogLevel::WARN, statusMessage_);
    return deleted;
}

std::vector<MazeRecord> MazeController::listMazes() const {
    return storage_.listMazes(canModerate());
}

bool MazeController::canCreateMaze() const {
    return UserManager::canCreateMaze(role_, blocked_);
}

bool MazeController::canSaveMaze() const {
    return UserManager::canSaveMaze(role_, blocked_);
}

bool MazeController::canModerate() const {
    return UserManager::canModerate(role_, blocked_);
}

const Grid& MazeController::grid() const {
    return grid_;
}

Point MazeController::start() const {
    return start_;
}

Point MazeController::finish() const {
    return finish_;
}

const AlgorithmResult& MazeController::lastResult() const {
    return lastResult_;
}

const std::string& MazeController::statusMessage() const {
    return statusMessage_;
}

Point MazeController::firstPassage() const {
    for (int y = 0; y < gridHeight(grid_); ++y) {
        for (int x = 0; x < gridWidth(grid_); ++x) {
            const Point point{x, y};
            if (isWalkable(grid_, point)) {
                return point;
            }
        }
    }
    return {0, 0};
}

Point MazeController::lastPassage() const {
    for (int y = gridHeight(grid_) - 1; y >= 0; --y) {
        for (int x = gridWidth(grid_) - 1; x >= 0; --x) {
            const Point point{x, y};
            if (isWalkable(grid_, point)) {
                return point;
            }
        }
    }
    return firstPassage();
}

void MazeController::overlayEndpoints() {
    if (isInside(grid_, start_)) {
        grid_[start_.y][start_.x] = CellState::Start;
    }
    if (isInside(grid_, finish_)) {
        grid_[finish_.y][finish_.x] = CellState::Finish;
    }
}

bool MazeController::isValidEndpoint(Point point) const {
    return isWalkable(grid_, point);
}

void MazeController::setStatus(std::string message) {
    statusMessage_ = std::move(message);
}
