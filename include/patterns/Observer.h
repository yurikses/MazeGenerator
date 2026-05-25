#pragma once

#include "patterns/algorithm.h"

#include <string>
#include <vector>

class RecordingObserver : public IProgressObserver {
public:
    void onStepUpdated(const VisualizationData& data) override;
    void clear();

    std::vector<std::string> messages;
    std::vector<Point> currents;
};
