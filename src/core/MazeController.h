#pragma once
#include "../model/Grid.h"
#include "../algorithm/AlgorithmFactory.h"
#include "../observer/IObserver.h"
#include <memory>
class MazeController {
    Grid grid; std::unique_ptr<IAlgorithm> algo; int sx=-1, sy=-1, ex=-1, ey=-1;
    bool running = false; std::shared_ptr<IObserver> guiObs;
public:
    MazeController(int w=30, int h=20) : grid(w,h) {}
    void resize(int w, int h); void setPoints(int startX, int startY, int endX, int endY);
    void run(AlgoType t); bool step(); bool isRunning() const;
    void setObserver(std::shared_ptr<IObserver> o);
    Grid& getGrid(); const Grid& getGrid() const;
    void save(const std::string& id, bool pub);
};
