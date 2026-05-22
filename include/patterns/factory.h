#pragma once

#include "core/types.h"
#include "patterns/algorithm.h"

#include <memory>

class AlgorithmFactory {
public:
    static std::unique_ptr<IMazeAlgorithm> create(AlgorithmType type);
    static bool isGenerator(AlgorithmType type);
    static bool isPathfinder(AlgorithmType type);
};
