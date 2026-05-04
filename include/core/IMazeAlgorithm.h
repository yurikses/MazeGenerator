#pragma once
namespace maze::core {
    class IMazeAlgorithm {
    public:
        virtual ~IMazeAlgorithm() = default;
        virtual void generate() = 0;
    };
}
