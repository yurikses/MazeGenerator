#include "patterns/Observer.h"

void RecordingObserver::onStepUpdated(const VisualizationData& data) {
    messages.push_back(data.message);
    currents.push_back(data.current);
}

void RecordingObserver::clear() {
    messages.clear();
    currents.clear();
}
