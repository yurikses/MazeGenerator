#pragma once
#include "IObserver.h"
#include <vector>
#include <memory>
class Subject {
    std::vector<std::shared_ptr<IObserver>> observers;
public:
    void attach(std::shared_ptr<IObserver> o);
    void detach(std::shared_ptr<IObserver> o);
    void notify(const VisData& d);
};
