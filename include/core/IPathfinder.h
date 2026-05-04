#pragma once
namespace maze::core {
    class IPathfinder {
    public:
        virtual ~IPathfinder() = default;
        virtual void findPath() = 0;
    };
}
