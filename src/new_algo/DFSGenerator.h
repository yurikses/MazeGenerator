#pragma once
#include "IAlgorithm.h"
#include <stack>

class DFSGenerator : public IAlgorithm {
    Grid* grid = nullptr;
    std::stack<std::pair<int,int>> stk;
public:
    void reset(Grid& g, int sx, int sy, int ex, int ey) override;
    bool step() override;
    std::string name() const override { return "DFS Generator"; }
};
