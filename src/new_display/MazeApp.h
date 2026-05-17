#pragma once
#include <memory>
#include "../new_core/MazeController.h"
#include "../new_user/AuthManager.h"
#include "../new_algo/AlgorithmFactory.h"
#include "Command.h"

// Модуль отображения: предоставляет UI на базе ImGui + SFML
//#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../observer/IObserver.h"
class MazeAppNew : public IObserver {
    MazeControllerFacade ctrl;
    bool running=false;
public:
    MazeAppNew();
    void onStep(const VisData& d) override;
    void run();
    // команды UI
    void executeCommand(std::unique_ptr<Command> cmd);
};
