#pragma once

#include "core/types.h"

#include <algorithm>
#include <vector>

class IProgressObserver {
public:
    virtual ~IProgressObserver() = default;
    virtual void onStepUpdated(const VisualizationData& data) = 0;
};

class IMazeAlgorithm {
public:
    virtual ~IMazeAlgorithm() = default;

    void attach(IProgressObserver* observer) {
        if (observer == nullptr) {
            return;
        }
        if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    void detach(IProgressObserver* observer) {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }

    virtual AlgorithmResult execute(Grid& maze, const Point& start, const Point& finish) = 0;
    virtual AlgorithmType type() const = 0;

protected:
    void notify(const VisualizationData& data) {
        for (auto* observer : observers_) {
            observer->onStepUpdated(data);
        }
    }

private:
    std::vector<IProgressObserver*> observers_;
};
