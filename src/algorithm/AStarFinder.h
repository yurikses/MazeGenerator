#pragma once
#include "IAlgorithm.h"
#include <queue>
#include <cmath>
class AStarFinder : public IAlgorithm {
    struct N{int x,y,f; bool operator>(const N&o)const{return f>o.f;}};
    std::priority_queue<N, std::vector<N>, std::greater<N>> pq;
    std::vector<std::vector<bool>> vis; std::vector<std::vector<std::pair<int,int>>> par;
    Grid* grid=nullptr; int ex=0, ey=0; const int dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};
public:
    std::string name() const override;
    void reset(Grid& g, int sx, int sy, int ex_, int ey_) override;
    bool step() override;
};