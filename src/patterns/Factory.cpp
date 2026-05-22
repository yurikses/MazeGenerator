#include "patterns/factory.h"

#include "algorithms/AStarFinder.h"
#include "algorithms/BFSFinder.h"
#include "algorithms/DFSGenerator.h"
#include "algorithms/KruskalGenerator.h"

#include <stdexcept>

std::unique_ptr<IMazeAlgorithm> AlgorithmFactory::create(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::DFSGenerator:
            return std::make_unique<DFSGenerator>();
        case AlgorithmType::KruskalGenerator:
            return std::make_unique<KruskalGenerator>();
        case AlgorithmType::BFSPathfinder:
            return std::make_unique<BFSFinder>();
        case AlgorithmType::AStarPathfinder:
            return std::make_unique<AStarFinder>();
    }

    throw std::invalid_argument("Unsupported algorithm type");
}

bool AlgorithmFactory::isGenerator(AlgorithmType type) {
    return type == AlgorithmType::DFSGenerator || type == AlgorithmType::KruskalGenerator;
}

bool AlgorithmFactory::isPathfinder(AlgorithmType type) {
    return type == AlgorithmType::BFSPathfinder || type == AlgorithmType::AStarPathfinder;
}
