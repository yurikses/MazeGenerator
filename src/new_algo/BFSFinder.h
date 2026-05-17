#pragma once
#include "IAlgorithm.h"
#include <queue>
#include <vector>

class BFSFinder : public IAlgorithm {
    Grid* grid = nullptr;
    std::queue<std::pair<int,int>> q;
    std::vector<std::vector<bool>> vis;
    std::vector<std::vector<std::pair<int,int>>> par;
    int ex=0, ey=0;
public:
    void reset(Grid& g, int sx, int sy, int ex_, int ey_) override;
    bool step() override;
    std::string name() const override { return "BFS Finder"; }
};
