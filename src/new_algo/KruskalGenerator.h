#pragma once
#include "IAlgorithm.h"
#include <vector>
#include <numeric>
#include <memory>

struct Edge { int u,v; int dir; };

class KruskalGenerator : public IAlgorithm {
    Grid* grid=nullptr;
    std::vector<Edge> edges;
    size_t idx=0;
    struct DSU { std::vector<int> p; DSU(int n):p(n){ std::iota(p.begin(),p.end(),0);} int find(int a){return p[a]==a?a:p[a]=find(p[a]);} bool unite(int a,int b){a=find(a);b=find(b); if(a==b) return false; p[a]=b; return true;} };
    std::unique_ptr<DSU> dsu;
public:
    void reset(Grid& g, int sx, int sy, int ex, int ey) override;
    bool step() override;
    std::string name() const override { return "Kruskal Generator"; }
};
