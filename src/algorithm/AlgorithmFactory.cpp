#include "AlgorithmFactory.h"
#include "DFSGenerator.h"
#include "KruskalGenerator.h"
#include "BFSFinder.h"
#include "AStarFinder.h"
std::unique_ptr<IAlgorithm> AlgorithmFactory::create(AlgoType t, Grid& g) {
    switch(t) {
        case AlgoType::DFS: return std::make_unique<DFSGenerator>();
        case AlgoType::KRUSKAL: return std::make_unique<KruskalGenerator>();
        case AlgoType::BFS: return std::make_unique<BFSFinder>();
        case AlgoType::ASTAR: return std::make_unique<AStarFinder>();
    }
    return nullptr;
}