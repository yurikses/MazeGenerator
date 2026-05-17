#include "Subject.h"
#include <algorithm>
void Subject::attach(std::shared_ptr<IObserver> o) { observers.push_back(o); }
void Subject::detach(std::shared_ptr<IObserver> o) { observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end()); }
void Subject::notify(const VisData& d) { for(auto& o : observers) o->onStep(d); }
