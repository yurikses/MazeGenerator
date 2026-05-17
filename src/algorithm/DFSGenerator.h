#pragma once
#include "IAlgorithm.h"
#include <stack>
#include <random>
class DFSGenerator : public IAlgorithm {
    std::stack<std::pair<int,int>> stk;
    std::mt19937 rng{std::random_device{}()};
    Grid* grid = nullptr;
    const int dirs[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
public:
    std::string name() const override;
    void reset(Grid& g, int sx, int sy, int ex, int ey) override;
    bool step() override;
};
