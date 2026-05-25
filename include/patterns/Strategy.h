#pragma once

#include "patterns/algorithm.h"

#include <string>
#include <vector>

class NotifyingAlgorithm final : public IMazeAlgorithm {
public:
    NotifyingAlgorithm();
    explicit NotifyingAlgorithm(std::vector<VisualizationData> steps);

    AlgorithmResult execute(Grid& maze, const Point& start, const Point& finish) override;
    AlgorithmType type() const override;

private:
    std::vector<VisualizationData> steps_;
};
