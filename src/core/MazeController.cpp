#include "MazeController.h"
#include "AppLogger.h"
#include "MazeStorage.h"
#include "UserManager.h"

void MazeController::resize(int w, int h) { grid.resize(w,h); }
void MazeController::setPoints(int startX, int startY, int endX, int endY) { sx=startX; sy=startY; ex=endX; ey=endY; }
void MazeController::run(AlgoType t) {
    algo = AlgorithmFactory::create(t, grid);
    if(algo) { algo->reset(grid, sx, sy, ex, ey); if(guiObs) algo->attach(guiObs); running=true; }
}
bool MazeController::step() { if(running && algo) { bool r=algo->step(); if(!r) running=false; return r; } return false; }
bool MazeController::isRunning() const { return running; }
void MazeController::setObserver(std::shared_ptr<IObserver> o) { guiObs=o; }
Grid& MazeController::getGrid() { return grid; }
const Grid& MazeController::getGrid() const { return grid; }


void MazeController::save(const std::string& id, bool pub) {
    MazeData m{id, UserManager::instance().name(), grid.W, grid.H, pub};
    MazeStorage::save(m);
    AppLogger::instance().log("Maze saved: " + id);
}