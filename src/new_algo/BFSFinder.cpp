#include "BFSFinder.h"

void BFSFinder::reset(Grid& g, int sx, int sy, int ex_, int ey_) {
    grid = &g; ex=ex_; ey=ey_;
    while(!q.empty()) q.pop();
    vis.assign(grid->H, std::vector<bool>(grid->W,false));
    q.push({sx,sy}); vis[sy][sx]=true;
    par.assign(grid->H, std::vector<std::pair<int,int>>(grid->W, {-1,-1}));
}

bool BFSFinder::step() {
    if(!grid || q.empty()) return false;
    auto [x,y]=q.front(); q.pop();
    notify(VisData{x,y,0});
    const int dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};
    for(auto &d: dirs) {
        int nx=x+d[0], ny=y+d[1];
        if(nx>=0 && nx<grid->W && ny>=0 && ny<grid->H) {
            if(!vis[ny][nx]) { vis[ny][nx]=true; q.push({nx,ny}); par[ny][nx] = {x,y}; }
        }
    }
    // if reached end, reconstruct path
    if(x==ex && y==ey) {
        // reconstruct
        int cx = x, cy = y;
        while(!(cx== -1 && cy== -1)) {
            grid->at(cx,cy).status = 3; // path
            auto p = par[cy][cx]; cx = p.first; cy = p.second;
        }
        notify(VisData{x,y,1});
        return false;
    }
    return !q.empty();
}
