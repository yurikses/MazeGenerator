#include "AlgorithmFactory.h"
#include "DFSGenerator.h"
#include "KruskalGenerator.h"
#include "BFSFinder.h"
#include "AStarFinder.h"

std::unique_ptr<IAlgorithm> AlgorithmFactory::create(AlgoTypeNew t) {
    switch(t) {
        case AlgoTypeNew::DFS: return std::make_unique<DFSGenerator>();
        case AlgoTypeNew::KRUSKAL: return std::make_unique<KruskalGenerator>();
        case AlgoTypeNew::BFS: return std::make_unique<BFSFinder>();
        case AlgoTypeNew::ASTAR: return std::make_unique<AStarFinder>();
    }
    return nullptr;
}
