#include "patterns/algorithm.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {
class RecordingObserver final : public IProgressObserver {
public:
    void onStepUpdated(const VisualizationData& data) override {
        messages.push_back(data.message);
        currents.push_back(data.current);
    }

    std::vector<std::string> messages;
    std::vector<Point> currents;
};

class NotifyingAlgorithm final : public IMazeAlgorithm {
public:
    AlgorithmResult execute(Grid&, const Point&, const Point&) override {
        notify({{1, 1}, {}, {}, "first"});
        notify({{2, 1}, {}, {}, "second"});
        notify({{3, 1}, {}, {}, "third"});
        return {true, {}, {}, "done", {}};
    }

    AlgorithmType type() const override {
        return AlgorithmType::DFSGenerator;
    }
};
} // namespace

TEST(ObserverPattern, AttachedObserverReceivesNotifications) {
    NotifyingAlgorithm algorithm;
    RecordingObserver observer;
    Grid grid;

    algorithm.attach(&observer);
    algorithm.execute(grid, {}, {});

    ASSERT_EQ(observer.messages.size(), 3U);
    EXPECT_EQ(observer.messages.front(), "first");
    EXPECT_EQ(observer.currents.back(), Point({3, 1}));
}

TEST(ObserverPattern, DuplicateObserverIsAttachedOnlyOnce) {
    NotifyingAlgorithm algorithm;
    RecordingObserver observer;
    Grid grid;

    algorithm.attach(&observer);
    algorithm.attach(&observer);
    algorithm.execute(grid, {}, {});

    EXPECT_EQ(observer.messages.size(), 3U);
}

TEST(ObserverPattern, DetachedObserverStopsReceivingNotifications) {
    NotifyingAlgorithm algorithm;
    RecordingObserver observer;
    Grid grid;

    algorithm.attach(&observer);
    algorithm.detach(&observer);
    algorithm.execute(grid, {}, {});

    EXPECT_TRUE(observer.messages.empty());
}

TEST(ObserverPattern, NullObserverIsIgnored) {
    NotifyingAlgorithm algorithm;
    Grid grid;

    EXPECT_NO_THROW(algorithm.attach(nullptr));
    EXPECT_NO_THROW(algorithm.execute(grid, {}, {}));
}

TEST(ObserverPattern, MultipleObserversReceiveTheSameSteps) {
    NotifyingAlgorithm algorithm;
    RecordingObserver first;
    RecordingObserver second;
    Grid grid;

    algorithm.attach(&first);
    algorithm.attach(&second);
    algorithm.execute(grid, {}, {});

    EXPECT_EQ(first.messages, second.messages);
    EXPECT_EQ(first.currents.size(), second.currents.size());
}

TEST(ObserverPattern, DetachingUnknownObserverIsSafe) {
    NotifyingAlgorithm algorithm;
    RecordingObserver first;
    RecordingObserver second;
    Grid grid;

    algorithm.attach(&first);
    algorithm.detach(&second);
    algorithm.execute(grid, {}, {});

    EXPECT_EQ(first.messages.size(), 3U);
    EXPECT_TRUE(second.messages.empty());
}
