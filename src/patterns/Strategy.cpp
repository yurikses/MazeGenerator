#include "patterns/Strategy.h"

#include <utility>

NotifyingAlgorithm::NotifyingAlgorithm()
    : steps_{
          {{1, 1}, {}, {}, "first"},
          {{2, 1}, {}, {}, "second"},
          {{3, 1}, {}, {}, "third"},
      } {}

NotifyingAlgorithm::NotifyingAlgorithm(std::vector<VisualizationData> steps) : steps_(std::move(steps)) {}

AlgorithmResult NotifyingAlgorithm::execute(Grid&, const Point&, const Point&) {
    for (const VisualizationData& step : steps_) {
        notify(step);
    }

    AlgorithmResult result;
    result.success = true;
    result.message = "Notifications emitted";
    result.metrics.visitedCells = steps_.size();
    return result;
}

AlgorithmType NotifyingAlgorithm::type() const {
    return AlgorithmType::DFSGenerator;
}
