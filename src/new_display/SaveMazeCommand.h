#pragma once
#include "Command.h"
#include "../new_core/MazeController.h"
#include <string>

// Команда сохранения лабиринта
class SaveMazeCommand : public Command {
    MazeControllerFacade& ctrl;
    std::string id;
    bool pub = false;
public:
    SaveMazeCommand(MazeControllerFacade& c, const std::string& id_, bool p=false) : ctrl(c), id(id_), pub(p) {}
    void execute() override { ctrl.save(id, pub); }
};
