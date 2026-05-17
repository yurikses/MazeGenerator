#include "KruskalGenerator.h"
#include <numeric>

void KruskalGenerator::reset(Grid& g, int sx, int sy, int ex, int ey) {
    grid=&g; edges.clear(); idx=0;
    // строим список рёбер в сетке (упрощённо)
    for(int y=0;y<g.H;y++) for(int x=0;x<g.W;x++) {
        if(x+1<g.W) edges.push_back({y*g.W + x, y*g.W + (x+1), 0});
        if(y+1<g.H) edges.push_back({y*g.W + x, (y+1)*g.W + x, 1});
    }
        dsu = std::make_unique<DSU>(g.W * g.H);
}

struct DSU { std::vector<int> p;
    DSU(int n):p(n){ std::iota(p.begin(), p.end(), 0); }
    int find(int a){ return p[a]==a?a:p[a]=find(p[a]); }
    bool unite(int a,int b){ a=find(a); b=find(b); if(a==b) return false; p[a]=b; return true; }
};

bool KruskalGenerator::step() {
    if(!dsu) dsu = std::make_unique<DSU>(grid->W * grid->H);
    while(idx < edges.size()) {
        auto &e = edges[idx++];
        if(dsu->unite(e.u, e.v)) {
            int ux = e.u % grid->W, uy = e.u / grid->W;
            int vx = e.v % grid->W, vy = e.v / grid->W;
            // remove wall between
            int dir = (vx==ux? (vy>uy?2:0) : (vx>ux?1:3));
            grid->at(ux,uy).walls[dir]=false;
            grid->at(vx,vy).walls[(dir+2)%4]=false;
            notify(VisData{ux,uy,0});
            notify(VisData{vx,vy,0});
            return idx<edges.size();
        }
    }
    notify(VisData{0,0,1});
    dsu.reset();
    return false;
}
