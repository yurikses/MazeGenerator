#include "ui/MazeApp.h"

#include "core/AppLogger.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <algorithm>
#include <cstdio>

namespace {
int generatorIndex(AlgorithmType type) {
    return type == AlgorithmType::KruskalGenerator ? 1 : 0;
}

AlgorithmType generatorFromIndex(int index) {
    return index == 1 ? AlgorithmType::KruskalGenerator : AlgorithmType::DFSGenerator;
}

int pathfinderIndex(AlgorithmType type) {
    return type == AlgorithmType::AStarPathfinder ? 1 : 0;
}

AlgorithmType pathfinderFromIndex(int index) {
    return index == 1 ? AlgorithmType::AStarPathfinder : AlgorithmType::BFSPathfinder;
}
} // namespace

MazeApp::MazeApp() : storage_("maze_app.db"), controller_(storage_) {
    std::snprintf(mazeName_, sizeof(mazeName_), "Maze");
    applyCurrentUser();
    refreshMazeList();
    visibleVisited_ = 0;
    visiblePath_ = 0;
}

int MazeApp::run() {
    sf::RenderWindow window(sf::VideoMode({1280, 760}), "Maze Generator & Path Finder");
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        AppLogger::getInstance().log(LogLevel::ERROR, "Failed to initialize ImGui-SFML");
        return 1;
    }

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            handleEvent(*event, window);
        }

        const sf::Time deltaTime = deltaClock.restart();
        advanceAnimation(deltaTime.asSeconds());
        ImGui::SFML::Update(window, deltaTime);

        const sf::Vector2u windowSize = window.getSize();
        const float controlWidth = 340.0F;
        mazeView_.setViewport({{controlWidth + 18.0F, 18.0F},
                               {std::max(100.0F, static_cast<float>(windowSize.x) - controlWidth - 36.0F),
                                std::max(100.0F, static_cast<float>(windowSize.y) - 36.0F)}});

        renderControls();

        window.clear(sf::Color(12, 15, 20));
        const auto& result = controller_.lastResult();
        mazeView_.draw(window,
                       controller_.grid(),
                       controller_.start(),
                       controller_.finish(),
                       result.visited,
                       visibleVisited_,
                       result.path,
                       visiblePath_);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

void MazeApp::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::Closed>()) {
        window.close();
        return;
    }

    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        window.setView(sf::View(sf::FloatRect({0.0F, 0.0F},
                                              {static_cast<float>(resized->size.x),
                                               static_cast<float>(resized->size.y)})));
        return;
    }

    const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>();
    if (mouse == nullptr || ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (mouse->button != sf::Mouse::Button::Left && mouse->button != sf::Mouse::Button::Right) {
        return;
    }

    const auto selectedCell = mazeView_.cellAt(mouse->position, controller_.grid());
    if (!selectedCell.has_value()) {
        return;
    }

    if (mouse->button == sf::Mouse::Button::Right || selectionMode_ == SelectionMode::Finish) {
        controller_.setFinish(*selectedCell);
    } else {
        controller_.setStart(*selectedCell);
    }

    visibleVisited_ = 0;
    visiblePath_ = 0;
    animationActive_ = false;
}

void MazeApp::renderControls() {
    const auto& users = userManager_.users();
    if (selectedUserIndex_ < 0 || selectedUserIndex_ >= static_cast<int>(users.size())) {
        selectedUserIndex_ = 0;
        applyCurrentUser();
    }

    ImGui::SetNextWindowPos({0.0F, 0.0F}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({340.0F, ImGui::GetIO().DisplaySize.y}, ImGuiCond_Always);
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    const UserAccount& currentUser = users[static_cast<std::size_t>(selectedUserIndex_)];
    if (ImGui::BeginCombo("Account", currentUser.name.c_str())) {
        for (int index = 0; index < static_cast<int>(users.size()); ++index) {
            const bool selected = index == selectedUserIndex_;
            const std::string label = users[static_cast<std::size_t>(index)].name + " / " +
                                      roleName(users[static_cast<std::size_t>(index)].role);
            if (ImGui::Selectable(label.c_str(), selected)) {
                selectedUserIndex_ = index;
                applyCurrentUser();
                refreshMazeList();
            }
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Role: %s%s", roleName(currentUser.role), currentUser.blocked ? " (blocked)" : "");
    ImGui::Separator();

    int genIndex = generatorIndex(generatorType_);
    ImGui::Combo("Generator", &genIndex, "DFS\0Kruskal\0");
    generatorType_ = generatorFromIndex(genIndex);
    ImGui::SliderInt("Width", &width_, 5, 101);
    ImGui::SliderInt("Height", &height_, 5, 101);

    if (!controller_.canCreateMaze()) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Generate", {-1.0F, 0.0F})) {
        if (controller_.generateMaze(generatorType_, width_, height_)) {
            startAnimation();
        }
    }
    if (!controller_.canCreateMaze()) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();

    int pathIndex = pathfinderIndex(pathfinderType_);
    ImGui::Combo("Pathfinder", &pathIndex, "BFS\0A*\0");
    pathfinderType_ = pathfinderFromIndex(pathIndex);

    int startInput[2] = {controller_.start().x, controller_.start().y};
    if (ImGui::InputInt2("Start x/y", startInput)) {
        controller_.setStart({startInput[0], startInput[1]});
    }

    int finishInput[2] = {controller_.finish().x, controller_.finish().y};
    if (ImGui::InputInt2("Finish x/y", finishInput)) {
        controller_.setFinish({finishInput[0], finishInput[1]});
    }

    int modeIndex = selectionMode_ == SelectionMode::Start ? 0 : 1;
    ImGui::RadioButton("Click sets start", &modeIndex, 0);
    ImGui::RadioButton("Click sets finish", &modeIndex, 1);
    selectionMode_ = modeIndex == 0 ? SelectionMode::Start : SelectionMode::Finish;

    if (currentUser.blocked) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Find path", {-1.0F, 0.0F})) {
        if (controller_.findPath(pathfinderType_)) {
            startAnimation();
        }
    }
    if (currentUser.blocked) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    ImGui::InputText("Name", mazeName_, sizeof(mazeName_));
    ImGui::Checkbox("Published", &publishOnSave_);
    if (!controller_.canSaveMaze()) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Save maze", {-1.0F, 0.0F})) {
        if (controller_.saveCurrentMaze(mazeName_, publishOnSave_)) {
            refreshMazeList();
        }
    }
    if (!controller_.canSaveMaze()) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    renderMazeLibrary();

    if (controller_.canModerate()) {
        ImGui::Separator();
        renderAdminPanel();
    }

    const AlgorithmResult& result = controller_.lastResult();
    ImGui::Separator();
    ImGui::TextWrapped("Status: %s", controller_.statusMessage().c_str());
    ImGui::Text("Visited: %zu", result.metrics.visitedCells);
    ImGui::Text("Path: %zu", result.metrics.pathLength);
    ImGui::Text("Time: %.3f ms", result.metrics.elapsedMs);

    ImGui::End();
}

void MazeApp::renderMazeLibrary() {
    if (ImGui::Button("Refresh library", {-1.0F, 0.0F})) {
        refreshMazeList();
    }

    ImGui::BeginChild("MazeLibrary", {0.0F, 170.0F}, ImGuiChildFlags_Border);
    for (const MazeRecord& record : mazeRecords_) {
        ImGui::PushID(record.id);
        ImGui::Text("#%d %s", record.id, record.name.c_str());
        ImGui::Text("%dx%d, %s, %s", record.width, record.height, record.owner.c_str(),
                    record.published ? "public" : "private");
        if (ImGui::Button("Load")) {
            controller_.loadMaze(record.id);
            visibleVisited_ = 0;
            visiblePath_ = 0;
            animationActive_ = false;
        }
        if (controller_.canModerate()) {
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                if (controller_.deleteMaze(record.id)) {
                    refreshMazeList();
                }
            }
        }
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();
}

void MazeApp::renderAdminPanel() {
    ImGui::Text("Users");
    for (const UserAccount& user : userManager_.users()) {
        ImGui::PushID(user.name.c_str());
        ImGui::Text("%s / %s%s", user.name.c_str(), roleName(user.role), user.blocked ? " / blocked" : "");
        if (user.role != UserRole::Admin) {
            ImGui::SameLine();
            if (ImGui::Button(user.blocked ? "Unblock" : "Block")) {
                userManager_.setBlocked(user.name, !user.blocked);
                applyCurrentUser();
            }
        }
        ImGui::PopID();
    }
}

void MazeApp::refreshMazeList() {
    mazeRecords_ = controller_.listMazes();
}

void MazeApp::applyCurrentUser() {
    const auto& users = userManager_.users();
    if (users.empty()) {
        return;
    }

    selectedUserIndex_ = std::clamp(selectedUserIndex_, 0, static_cast<int>(users.size()) - 1);
    const UserAccount& user = users[static_cast<std::size_t>(selectedUserIndex_)];
    controller_.setAccessContext(user.name, user.role, user.blocked);
}

void MazeApp::startAnimation() {
    visibleVisited_ = 0;
    visiblePath_ = 0;
    animationAccumulator_ = 0.0F;
    animationActive_ = true;
}

void MazeApp::advanceAnimation(float deltaSeconds) {
    if (!animationActive_) {
        return;
    }

    const AlgorithmResult& result = controller_.lastResult();
    animationAccumulator_ += deltaSeconds;
    while (animationAccumulator_ >= animationStepSeconds_) {
        animationAccumulator_ -= animationStepSeconds_;

        if (visibleVisited_ < result.visited.size()) {
            ++visibleVisited_;
        } else if (visiblePath_ < result.path.size()) {
            ++visiblePath_;
        } else {
            animationActive_ = false;
            break;
        }
    }
}
