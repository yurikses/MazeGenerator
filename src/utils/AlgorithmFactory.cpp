#include "utils/AlgorithmFactory.h"

namespace maze::utils {

std::unique_ptr<core::IMazeAlgorithm> AlgorithmFactory::createGenerator(int type) {
    // TODO: фабричный метод
    return nullptr;
}

std::unique_ptr<core::IPathfinder> AlgorithmFactory::createPathfinder(int type) {
    // TODO: фабричный метод
    return nullptr;
}

} // namespace maze::utils