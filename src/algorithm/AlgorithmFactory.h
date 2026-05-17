#pragma once
#include "IAlgorithm.h"
enum class AlgoType { DFS, KRUSKAL, BFS, ASTAR };
class AlgorithmFactory {
public:
    static std::unique_ptr<IAlgorithm> create(AlgoType t, Grid& g);
};