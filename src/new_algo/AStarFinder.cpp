#include "AStarFinder.h"
#include <cmath>

static int heuristic(int x1,int y1,int x2,int y2) { return std::abs(x1-x2)+std::abs(y1-y2); }

void AStarFinder::reset(Grid& g, int sx, int sy, int ex_, int ey_) {
    grid=&g; ex=ex_; ey=ey_; sx_=sx; sy_=sy;
    while(!open.empty()) open.pop();
    gscore.assign(grid->H, std::vector<int>(grid->W, INT_MAX));
    par.assign(grid->H, std::vector<std::pair<int,int>>(grid->W, {-1,-1}));
    closed.assign(grid->H, std::vector<bool>(grid->W,false));
    gscore[sy][sx]=0;
    open.push(Node{sx,sy,0,heuristic(sx,sy,ex,ey)});
}

bool AStarFinder::step() {
    if(!grid || open.empty()) return false;
    Node cur = open.top(); open.pop();
    if(closed[cur.y][cur.x]) return !open.empty();
    closed[cur.y][cur.x]=true;
    notify(VisData{cur.x,cur.y,0});
    if(cur.x==ex && cur.y==ey) {
        // reconstruct
        int cx=ex, cy=ey;
        while(!(cx==-1 && cy==-1)) {
            grid->at(cx,cy).status = 3;
            auto p = par[cy][cx]; cx=p.first; cy=p.second;
        }
        notify(VisData{ex,ey,1});
        return false;
    }
    const int dirs[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
    for(auto &d: dirs) {
        int nx=cur.x+d[0], ny=cur.y+d[1];
        if(nx<0||nx>=grid->W||ny<0||ny>=grid->H) continue;
        if(closed[ny][nx]) continue;
        int tentative = cur.g + 1;
        if(tentative < gscore[ny][nx]) {
            gscore[ny][nx]=tentative;
            par[ny][nx] = {cur.x, cur.y};
            int f = tentative + heuristic(nx,ny,ex,ey);
            open.push(Node{nx,ny,tentative,f});
        }
    }
    return !open.empty();
}
