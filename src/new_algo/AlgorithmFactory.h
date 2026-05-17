#pragma once
#include <memory>
#include "IAlgorithm.h"
#include "../model/Grid.h"

enum class AlgoTypeNew { DFS, KRUSKAL, BFS, ASTAR };

// Factory pattern: создаёт алгоритм по типу
class AlgorithmFactory {
public:
    static std::unique_ptr<IAlgorithm> create(AlgoTypeNew t);
};
