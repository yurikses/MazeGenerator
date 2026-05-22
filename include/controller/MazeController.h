#pragma once

#include "core/MazeStorage.h"
#include "core/types.h"

#include <string>
#include <vector>

class MazeController {
public:
    explicit MazeController(MazeStorage& storage);

    void setAccessContext(std::string userName, UserRole role, bool blocked);

    bool generateMaze(AlgorithmType generatorType, int width, int height);
    bool findPath(AlgorithmType pathfinderType);
    bool setStart(Point point);
    bool setFinish(Point point);
    void clearPath();

    bool saveCurrentMaze(const std::string& name, bool published);
    bool loadMaze(int id);
    bool deleteMaze(int id);
    std::vector<MazeRecord> listMazes() const;

    bool canCreateMaze() const;
    bool canSaveMaze() const;
    bool canModerate() const;

    const Grid& grid() const;
    Point start() const;
    Point finish() const;
    const AlgorithmResult& lastResult() const;
    const std::string& statusMessage() const;

private:
    Point firstPassage() const;
    Point lastPassage() const;
    void overlayEndpoints();
    bool isValidEndpoint(Point point) const;
    void setStatus(std::string message);

    MazeStorage& storage_;
    Grid grid_;
    Point start_{1, 1};
    Point finish_{1, 1};
    AlgorithmResult lastResult_;
    std::string statusMessage_{"Ready"};
    std::string userName_{"student"};
    UserRole role_{UserRole::User};
    bool blocked_{false};
};
