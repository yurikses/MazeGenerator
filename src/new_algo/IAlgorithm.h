#pragma once
#include <string>
#include "../model/Grid.h"
#include "../observer/Subject.h"

// Strategy pattern: разные алгоритмы (генерация / поиск) реализуют этот интерфейс
// Наследуем Subject, чтобы алгоритмы могли уведомлять наблюдателей о шагах
class IAlgorithm : public Subject {
public:
    virtual ~IAlgorithm() = default;
    virtual void reset(Grid& g, int sx, int sy, int ex, int ey) = 0;
    virtual bool step() = 0; // однократный шаг анимации/алгоритма
    virtual std::string name() const = 0;
};
