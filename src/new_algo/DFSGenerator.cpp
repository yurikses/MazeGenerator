#include "DFSGenerator.h"
#include <random>

void DFSGenerator::reset(Grid& g, int sx, int sy, int ex, int ey) {
    grid = &g;
    grid->reset();
    while(!stk.empty()) stk.pop();
    stk.push({sx,sy});
}

bool DFSGenerator::step() {
    if(!grid) return false;
    if(stk.empty()) return false;
    auto [x,y] = stk.top(); stk.pop();
    auto &c = grid->at(x,y);
    c.visited = true;
    // уведомим наблюдателя о посещении клетки
    notify(VisData{x,y,0});
    // простая реализация: пометим соседей как открытые и добавим в стек
    const int dirsArr[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
    std::vector<std::pair<int,int>> nbrs;
    for(auto &d: dirsArr) {
        int nx=x+d[0], ny=y+d[1];
        if(nx>=0 && nx<grid->W && ny>=0 && ny<grid->H) {
            auto &nc = grid->at(nx,ny);
            if(!nc.visited) nbrs.push_back({nx,ny});
        }
    }
    // случайный порядок
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::shuffle(nbrs.begin(), nbrs.end(), rng);
    for(auto &p: nbrs) {
        int nx=p.first, ny=p.second;
        // удалим стену между (x,y) и (nx,ny)
        int dir = (nx==x? (ny>y?2:0) : (nx>x?1:3));
        grid->at(x,y).walls[dir]=false;
        grid->at(nx,ny).walls[(dir+2)%4]=false;
        stk.push({nx,ny});
    }
    if(stk.empty()) {
        notify(VisData{x,y,1});
    }
    return !stk.empty();
}
