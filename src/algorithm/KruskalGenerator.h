#pragma once
#include "IAlgorithm.h"
#include <vector>
#include <random>
#include <numeric>
struct Edge { int u, v, dir; };
struct DSU { std::vector<int> p; 
    DSU(int n):p(n){
        std::iota(p.begin(),p.end(),0);
    } 
    int find(int i){
        return p[i]==i?i:p[i]=find(p[i]);
    } 
    bool unite(int i,int j){
        i=find(i);j=find(j);
        if(i==j)return false;
        p[i]=j;
        return true;
    } 
};
class KruskalGenerator : public IAlgorithm {
    std::vector<Edge> edges; std::mt19937 rng{std::random_device{}()};
    Grid* grid = nullptr; std::unique_ptr<DSU> dsu; int idx = 0;
public:
    std::string name() const override;
    void reset(Grid& g, int, int, int, int) override;
    bool step() override;
};
