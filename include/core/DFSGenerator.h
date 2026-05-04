#pragma once
#include "IMazeAlgorithm.h"
namespace maze::core {
    class DFSGenerator : public IMazeAlgorithm {
    public:
        void generate() override;
    };
}
