#pragma once

#include "patterns/algorithm.h"

class BFSFinder final : public IMazeAlgorithm {
public:
    AlgorithmResult execute(Grid& maze, const Point& start, const Point& finish) override;
    AlgorithmType type() const override;
};
