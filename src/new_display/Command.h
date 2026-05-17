#pragma once

// Command pattern — абстракция для действий UI (сохранение, загрузка, запуск генер.)
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};
