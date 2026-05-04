#pragma once
#include "IPathfinder.h"
namespace maze::core {
    class AStarFinder : public IPathfinder {
    public:
        void findPath() override;
    };
}
