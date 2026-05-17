#pragma once
#include "IAlgorithm.h"
#include <queue>
#include <vector>

class AStarFinder : public IAlgorithm {
    struct Node { int x,y; int g; int f; bool operator>(const Node&o) const { return f>o.f; } };
    Grid* grid=nullptr; int ex=0, ey=0;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    std::vector<std::vector<int>> gscore;
    std::vector<std::vector<std::pair<int,int>>> par;
    std::vector<std::vector<bool>> closed;
    int sx_=0, sy_=0;
public:
    void reset(Grid& g, int sx, int sy, int ex_, int ey_) override;
    bool step() override;
    std::string name() const override { return "A* Finder"; }
};
