#include "MazeController.h"
#include "../core/MazeStorage.h"
#include "../new_user/AuthManager.h"

void MazeControllerFacade::run() {
    if(algo) {
        for (auto &row : grid.data) {
            for (auto &cell : row) {
                cell.visited = false;
                cell.status = 0;
                cell.g = 0;
                cell.h = 0;
                cell.f = 0;
            }
        }
        if(guiObs) algo->detach(guiObs);
        algo->reset(grid, sx, sy, ex, ey);
        // если у нас есть GUI observer, привяжем его к алгоритму
        if(guiObs) algo->attach(guiObs);
        running = true;
    }
}

bool MazeControllerFacade::step() {
    if(!running && algo) {
        run();
    }
    if(running && algo) {
        bool more = algo->step();
        if(!more) running = false;
        return more;
    }
    return false;
}

void MazeControllerFacade::save(const std::string& id, bool pub) {
    MazeData m{id, AuthManager::instance().currentUser(), grid.W, grid.H, pub};
    // перенесём данные сетки в m.grid
    m.grid = grid.data;
    MazeStorage::save(m);
}

void MazeControllerFacade::stop() {
    running = false;
    if(algo && guiObs) {
        algo->detach(guiObs);
    }
}
