#pragma once
#include "IAlgorithm.h"
#include <queue>
class BFSFinder : public IAlgorithm {
    std::queue<std::pair<int,int>> q; std::vector<std::vector<bool>> vis;
    std::vector<std::vector<std::pair<int,int>>> par; Grid* grid=nullptr; int ex=0, ey=0;
    const int dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};
public:
    std::string name() const override;
    void reset(Grid& g, int sx, int sy, int ex_, int ey_) override;
    bool step() override;
};
