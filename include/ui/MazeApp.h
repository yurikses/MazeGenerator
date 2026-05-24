#pragma once

#include "controller/MazeController.h"
#include "core/MazeStorage.h"
#include "core/UserManager.h"
#include "core/types.h"
#include "ui/GUIMazeView.h"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

class MazeApp {
public:
    MazeApp();
    int run();

private:
    enum class Screen {
        Login,
        Main
    };

    enum class AnimationMode {
        None,
        Generation,
        Search
    };

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void renderLogin();
    void renderMainInterface();
    void renderUserTab();
    void renderGenerationTab();
    void renderMazesTab();
    void renderUsersTab();
    void renderStatusPanel();

    void refreshMazeList();
    void applyCurrentUser();
    void syncProfileBuffers();
    void logout();
    void startAnimation(AnimationMode mode);
    void stopAnimation();
    void advanceAnimation(float deltaSeconds);
    bool canRunPathSearch() const;
    const UserAccount* currentUser() const;

    MazeStorage storage_;
    MazeController controller_;
    UserManager userManager_;
    GUIMazeView mazeView_;

    Screen screen_{Screen::Login};
    int selectedUserIndex_{-1};
    char loginName_[64]{"student"};
    char loginPassword_[64]{"student"};
    char registerName_[64]{};
    char registerDisplayName_[64]{};
    char registerPassword_[64]{};
    char registerPasswordRepeat_[64]{};
    char authMessage_[160]{"Use student/student or admin/admin"};
    char profileDisplayName_[64]{};
    char profilePassword_[64]{};

    int width_{31};
    int height_{31};
    AlgorithmType generatorType_{AlgorithmType::DFSGenerator};
    AlgorithmType pathfinderType_{AlgorithmType::BFSPathfinder};
    SelectionMode selectionMode_{SelectionMode::Start};
    bool startSelected_{false};
    bool finishSelected_{false};

    bool publishOnSave_{true};
    char mazeName_[64]{"Maze"};
    std::vector<MazeRecord> mazeRecords_;

    bool animateGeneration_{true};
    bool animateSearch_{true};
    bool revealGeneration_{true};
    AnimationMode animationMode_{AnimationMode::None};
    float animationAccumulator_{0.0F};
    float animationStepSeconds_{0.012F};
    std::size_t visibleVisited_{0};
    std::size_t visiblePath_{0};
};
