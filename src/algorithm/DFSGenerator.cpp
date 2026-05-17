#include "DFSGenerator.h"
std::string DFSGenerator::name() const { return "DFS"; }
void DFSGenerator::reset(Grid& g, int sx, int sy, int, int) {
    grid = &g; g.reset(); stk = {}; g.at(sx,sy).visited = true; stk.push({sx,sy});
}
bool DFSGenerator::step() {
    if(stk.empty()) { notify({0,0,1}); return false; }
    auto [x,y] = stk.top();
    std::vector<std::pair<int,int>> nb;
    for(auto& d : dirs) {
        int nx = x+d[0], ny = y+d[1];
        if(nx>=0 && nx<grid->W && ny>=0 && ny<grid->H && !grid->at(nx,ny).visited) nb.push_back({nx,ny});
    }
    if(!nb.empty()) {
        auto [nx,ny] = nb[rng()%nb.size()];
        int dir = (nx>x)?1:(nx<x)?3:(ny>y)?2:0;
        grid->at(x,y).walls[dir] = grid->at(nx,ny).walls[(dir+2)%4] = false;
        grid->at(nx,ny).visited = true; stk.push({nx,ny});
        notify({nx,ny,0});
    } else stk.pop();
    return true;
}
