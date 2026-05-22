#pragma once

#include "controller/MazeController.h"
#include "core/MazeStorage.h"
#include "core/UserManager.h"
#include "core/types.h"
#include "ui/GUIMazeView.h"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <string>
#include <vector>

class MazeApp {
public:
    MazeApp();
    int run();

private:
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void renderControls();
    void renderMazeLibrary();
    void renderAdminPanel();
    void refreshMazeList();
    void applyCurrentUser();
    void startAnimation();
    void advanceAnimation(float deltaSeconds);

    MazeStorage storage_;
    MazeController controller_;
    UserManager userManager_;
    GUIMazeView mazeView_;

    int selectedUserIndex_{1};
    int width_{31};
    int height_{31};
    AlgorithmType generatorType_{AlgorithmType::DFSGenerator};
    AlgorithmType pathfinderType_{AlgorithmType::BFSPathfinder};
    SelectionMode selectionMode_{SelectionMode::Start};
    bool publishOnSave_{true};
    char mazeName_[64]{};
    std::vector<MazeRecord> mazeRecords_;

    bool animationActive_{false};
    float animationAccumulator_{0.0F};
    float animationStepSeconds_{0.012F};
    std::size_t visibleVisited_{0};
    std::size_t visiblePath_{0};
};
