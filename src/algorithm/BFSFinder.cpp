#include "BFSFinder.h"
std::string BFSFinder::name() const { return "BFS"; }
void BFSFinder::reset(Grid& g, int sx, int sy, int ex_, int ey_) {
    grid=&g; ex=ex_; ey=ey_; vis.assign(g.H, std::vector<bool>(g.W,false));
    par.assign(g.H, std::vector<std::pair<int,int>>(g.W,{-1,-1}));
    q={}; q.push({sx,sy}); vis[sy][sx]=true;
}
bool BFSFinder::step() {
    if(q.empty()){notify({0,0,1});return false;}
    auto [x,y]=q.front(); q.pop();
    if(x==ex && y==ey) { int cx=x,cy=y; while(cx!=-1){grid->at(cx,cy).status=3; auto p=par[cy][cx]; cx=p.first; cy=p.second;} notify({0,0,1}); return false; }
    grid->at(x,y).status=2;
    for(int i=0;i<4;i++){ int nx=x+dirs[i][0], ny=y+dirs[i][1];
        if(nx>=0&&nx<grid->W&&ny>=0&&ny<grid->H&&!vis[ny][nx]&&!grid->at(nx,ny).walls[i]) {
            vis[ny][nx]=true; par[ny][nx]={x,y}; grid->at(nx,ny).status=1; q.push({nx,ny}); }}
    return true;
}
