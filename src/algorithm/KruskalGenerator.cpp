#include "KruskalGenerator.h"
std::string KruskalGenerator::name() const { return "Kruskal"; }
void KruskalGenerator::reset(Grid& g, int, int, int, int) {
    grid=&g; g.reset(); edges.clear();
    for(int y=0;y<g.H;++y) for(int x=0;x<g.W;++x) {
        if(x<g.W-1) edges.push_back({y*g.W+x, y*g.W+x+1, 1});
        if(y<g.H-1) edges.push_back({y*g.W+x, (y+1)*g.W+x, 2});
    }
    std::shuffle(edges.begin(), edges.end(), rng);
    dsu = std::make_unique<DSU>(g.W*g.H); idx=0;
}
bool KruskalGenerator::step() {
    if(idx >= (int)edges.size()) { notify({0,0,1}); return false; }
    auto [u,v,d] = edges[idx++];
    int ux=u%grid->W, uy=u/grid->W, vx=v%grid->W, vy=v/grid->W;
    if(dsu->unite(u,v)) {
        grid->at(ux,uy).visited = grid->at(vx,vy).visited = true;
        int w1 = (vx>ux)?1:(vx<ux)?3:(vy>uy)?2:0;
        grid->at(ux,uy).walls[w1] = grid->at(vx,vy).walls[(w1+2)%4] = false;
        notify({vx,vy,0});
    }
    return true;
}