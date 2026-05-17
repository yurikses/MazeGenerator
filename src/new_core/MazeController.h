#pragma once
#include "../model/Grid.h"
#include "../new_algo/IAlgorithm.h"
#include <memory>
#include "../core/MazeStorage.h"
#include "../new_user/AuthManager.h"
#include <algorithm>

// Facade — объединяет функционал модулей: user, algorithms, display
// Предоставляет единый интерфейс для UI
class MazeControllerFacade {
    Grid grid;
    std::unique_ptr<IAlgorithm> algo;
    bool running = false;
    int sx=-1, sy=-1, ex=-1, ey=-1;
    std::shared_ptr<IObserver> guiObs;
public:
    MazeControllerFacade(int w=30, int h=20) : grid(w,h), sx(0), sy(0), ex(w > 0 ? w - 1 : 0), ey(h > 0 ? h - 1 : 0) {}
    Grid& getGrid() { return grid; }
    void resize(int w,int h) { grid.resize(w,h); sx = 0; sy = 0; ex = (w > 0 ? w - 1 : 0); ey = (h > 0 ? h - 1 : 0); }
    void setAlgorithm(std::unique_ptr<IAlgorithm> a) { algo = std::move(a); }
    void setPoints(int sx_, int sy_, int ex_, int ey_) {
        sx = std::clamp(sx_, 0, grid.W > 0 ? grid.W - 1 : 0);
        sy = std::clamp(sy_, 0, grid.H > 0 ? grid.H - 1 : 0);
        ex = std::clamp(ex_, 0, grid.W > 0 ? grid.W - 1 : 0);
        ey = std::clamp(ey_, 0, grid.H > 0 ? grid.H - 1 : 0);
    }
    void setObserver(std::shared_ptr<IObserver> o) { guiObs = o; }
    void run();
    void stop();
    bool step();
    // Сохранить текущее состояние лабиринта как MazeData
    void save(const std::string& id, bool pub);
    bool isRunning() const { return running; }
};
