#pragma once
#include "IPathfinder.h"
namespace maze::core {
    class BFSFinder : public IPathfinder {
    public:
        void findPath() override;
    };
}
