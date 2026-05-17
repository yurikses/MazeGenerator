#pragma once
struct VisData { int x, y, type; }; // type: 0=step, 1=finished
class IObserver { 
    public: 
        virtual ~IObserver() = default; 
        virtual void onStep(const VisData& d) = 0; 
};
