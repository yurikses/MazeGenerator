#include "algorithms/AStarFinder.h"
#include "algorithms/BFSFinder.h"
#include "algorithms/DFSGenerator.h"
#include "algorithms/KruskalGenerator.h"
#include "patterns/factory.h"

#include <gtest/gtest.h>

TEST(AlgorithmFactory, ClassifiesGeneratorTypes) {
    EXPECT_TRUE(AlgorithmFactory::isGenerator(AlgorithmType::DFSGenerator));
    EXPECT_TRUE(AlgorithmFactory::isGenerator(AlgorithmType::KruskalGenerator));
    EXPECT_FALSE(AlgorithmFactory::isGenerator(AlgorithmType::BFSPathfinder));
    EXPECT_FALSE(AlgorithmFactory::isGenerator(AlgorithmType::AStarPathfinder));
}

TEST(AlgorithmFactory, ClassifiesPathfinderTypes) {
    EXPECT_FALSE(AlgorithmFactory::isPathfinder(AlgorithmType::DFSGenerator));
    EXPECT_FALSE(AlgorithmFactory::isPathfinder(AlgorithmType::KruskalGenerator));
    EXPECT_TRUE(AlgorithmFactory::isPathfinder(AlgorithmType::BFSPathfinder));
    EXPECT_TRUE(AlgorithmFactory::isPathfinder(AlgorithmType::AStarPathfinder));
}

TEST(AlgorithmFactory, CreatesDfsGenerator) {
    const auto algorithm = AlgorithmFactory::create(AlgorithmType::DFSGenerator);

    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->type(), AlgorithmType::DFSGenerator);
    EXPECT_NE(dynamic_cast<DFSGenerator*>(algorithm.get()), nullptr);
}

TEST(AlgorithmFactory, CreatesKruskalGenerator) {
    const auto algorithm = AlgorithmFactory::create(AlgorithmType::KruskalGenerator);

    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->type(), AlgorithmType::KruskalGenerator);
    EXPECT_NE(dynamic_cast<KruskalGenerator*>(algorithm.get()), nullptr);
}

TEST(AlgorithmFactory, CreatesBfsPathfinder) {
    const auto algorithm = AlgorithmFactory::create(AlgorithmType::BFSPathfinder);

    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->type(), AlgorithmType::BFSPathfinder);
    EXPECT_NE(dynamic_cast<BFSFinder*>(algorithm.get()), nullptr);
}

TEST(AlgorithmFactory, CreatesAStarPathfinder) {
    const auto algorithm = AlgorithmFactory::create(AlgorithmType::AStarPathfinder);

    ASSERT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->type(), AlgorithmType::AStarPathfinder);
    EXPECT_NE(dynamic_cast<AStarFinder*>(algorithm.get()), nullptr);
}

TEST(AlgorithmFactory, CreatesIndependentInstances) {
    const auto first = AlgorithmFactory::create(AlgorithmType::DFSGenerator);
    const auto second = AlgorithmFactory::create(AlgorithmType::DFSGenerator);

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_NE(first.get(), second.get());
}
