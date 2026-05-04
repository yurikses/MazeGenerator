#pragma once
namespace maze::utils {
    class ProgressObserver {
    public:
        virtual ~ProgressObserver() = default;
        virtual void updateProgress(int percent) = 0;
    };
}
