#include "AStarFinder.h"
std::string AStarFinder::name() const { return "A*"; }
void AStarFinder::reset(Grid& g, int sx, int sy, int ex_, int ey_) {
    grid=&g; ex=ex_; ey=ey_; vis.assign(g.H, std::vector<bool>(g.W,false));
    par.assign(g.H, std::vector<std::pair<int,int>>(g.W,{-1,-1})); pq={};
    int h=std::abs(sx-ex)+std::abs(sy-ey); pq.push({sx,sy,h}); vis[sy][sx]=true;
}
bool AStarFinder::step() {
    if(pq.empty()){notify({0,0,1});return false;}
    auto c=pq.top(); pq.pop();
    if(c.x==ex && c.y==ey) { int cx=c.x,cy=c.y; while(cx!=-1){grid->at(cx,cy).status=3; auto p=par[cy][cx]; cx=p.first; cy=p.second;} notify({0,0,1}); return false; }
    grid->at(c.x,c.y).status=2;
    for(int i=0;i<4;i++){ int nx=c.x+dirs[i][0], ny=c.y+dirs[i][1];
        if(nx>=0&&nx<grid->W&&ny>=0&&ny<grid->H&&!vis[ny][nx]&&!grid->at(nx,ny).walls[i]) {
            vis[ny][nx]=true; par[ny][nx]={c.x,c.y};
            int g = grid->at(c.x,c.y).g + 1;
            pq.push({nx,ny, g + std::abs(nx-ex)+std::abs(ny-ey)});
            grid->at(nx,ny).status=1; grid->at(nx,ny).g=g; }}
    return true;
}
