#include "patterns/factory.h"

#include <gtest/gtest.h>

TEST(AlgorithmFactory, CreatesGeneratorsAndPathfinders) {
    EXPECT_TRUE(AlgorithmFactory::isGenerator(AlgorithmType::DFSGenerator));
    EXPECT_TRUE(AlgorithmFactory::isGenerator(AlgorithmType::KruskalGenerator));
    EXPECT_TRUE(AlgorithmFactory::isPathfinder(AlgorithmType::BFSPathfinder));
    EXPECT_TRUE(AlgorithmFactory::isPathfinder(AlgorithmType::AStarPathfinder));

    EXPECT_NE(AlgorithmFactory::create(AlgorithmType::DFSGenerator), nullptr);
    EXPECT_NE(AlgorithmFactory::create(AlgorithmType::KruskalGenerator), nullptr);
    EXPECT_NE(AlgorithmFactory::create(AlgorithmType::BFSPathfinder), nullptr);
    EXPECT_NE(AlgorithmFactory::create(AlgorithmType::AStarPathfinder), nullptr);
}
