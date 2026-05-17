#pragma once
#include "../observer/Subject.h"
#include "../model/Grid.h"
#include <string>
class IAlgorithm : public Subject {
public:
    virtual ~IAlgorithm() = default;
    virtual bool step() = 0;
    virtual void reset(Grid& g, int sx, int sy, int ex, int ey) = 0;
    virtual std::string name() const = 0;
};
