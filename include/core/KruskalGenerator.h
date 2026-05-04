#pragma once
#include "IMazeAlgorithm.h"
namespace maze::core {
    class KruskalGenerator : public IMazeAlgorithm {
    public:
        void generate() override;
    };
}
