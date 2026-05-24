#include "ui/MazeApp.h"

#include "core/AppLogger.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <string>

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

void copyToBuffer(char* buffer, std::size_t size, const std::string& value) {
    std::snprintf(buffer, size, "%s", value.c_str());
}

std::string trimmed(const char* value) {
    const std::string text = value == nullptr ? std::string{} : std::string{value};
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }

    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}
} // namespace

MazeApp::MazeApp() : storage_("maze_app.db"), controller_(storage_) {
    refreshMazeList();
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

        window.clear(sf::Color(12, 15, 20));

        if (screen_ == Screen::Main) {
            const sf::Vector2u windowSize = window.getSize();
            const float controlWidth = 380.0F;
            mazeView_.setViewport({{controlWidth + 18.0F, 18.0F},
                                   {std::max(100.0F, static_cast<float>(windowSize.x) - controlWidth - 36.0F),
                                    std::max(100.0F, static_cast<float>(windowSize.y) - 36.0F)}});

            const auto& result = controller_.lastResult();
            mazeView_.draw(window,
                           controller_.grid(),
                           controller_.start(),
                           startSelected_,
                           controller_.finish(),
                           finishSelected_,
                           result.visited,
                           visibleVisited_,
                           result.path,
                           visiblePath_,
                           animationMode_ == AnimationMode::Generation && revealGeneration_);
            renderMainInterface();
        } else {
            renderLogin();
        }

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

    if (screen_ != Screen::Main) {
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
        if (controller_.setFinish(*selectedCell)) {
            finishSelected_ = true;
        }
    } else if (controller_.setStart(*selectedCell)) {
        startSelected_ = true;
    }

    visibleVisited_ = 0;
    visiblePath_ = 0;
    stopAnimation();
}

void MazeApp::renderLogin() {
    const ImVec2 windowSize{380.0F, 330.0F};
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos({(displaySize.x - windowSize.x) * 0.5F, (displaySize.y - windowSize.y) * 0.5F},
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::Begin("Authorization", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("AuthTabs")) {
        if (ImGui::BeginTabItem("Sign in")) {
            ImGui::InputText("Login", loginName_, sizeof(loginName_));
            ImGui::InputText("Password", loginPassword_, sizeof(loginPassword_), ImGuiInputTextFlags_Password);

            if (ImGui::Button("Sign in", {-1.0F, 0.0F})) {
                const int userIndex = userManager_.authenticate(trimmed(loginName_), loginPassword_);
                if (userIndex < 0) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Invalid login or password");
                } else if (userManager_.users()[static_cast<std::size_t>(userIndex)].blocked) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Account is blocked");
                } else {
                    selectedUserIndex_ = userIndex;
                    applyCurrentUser();
                    syncProfileBuffers();
                    refreshMazeList();
                    screen_ = Screen::Main;
                    copyToBuffer(authMessage_, sizeof(authMessage_), "");
                }
            }

            if (ImGui::Button("Continue as guest", {-1.0F, 0.0F})) {
                const int guestIndex = userManager_.authenticate("guest", "");
                if (guestIndex >= 0 && userManager_.users()[static_cast<std::size_t>(guestIndex)].blocked) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Guest account is blocked");
                } else {
                    selectedUserIndex_ = guestIndex >= 0 ? guestIndex : 0;
                    applyCurrentUser();
                    syncProfileBuffers();
                    refreshMazeList();
                    screen_ = Screen::Main;
                    copyToBuffer(authMessage_, sizeof(authMessage_), "");
                }
            }

            ImGui::Separator();
            ImGui::Text("student / student");
            ImGui::Text("admin / admin");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Register")) {
            ImGui::InputText("New login", registerName_, sizeof(registerName_));
            ImGui::InputText("Display name", registerDisplayName_, sizeof(registerDisplayName_));
            ImGui::InputText("Password", registerPassword_, sizeof(registerPassword_), ImGuiInputTextFlags_Password);
            ImGui::InputText("Repeat password",
                             registerPasswordRepeat_,
                             sizeof(registerPasswordRepeat_),
                             ImGuiInputTextFlags_Password);

            if (ImGui::Button("Create account", {-1.0F, 0.0F})) {
                const std::string userName = trimmed(registerName_);
                const std::string displayName = trimmed(registerDisplayName_);
                const std::string password = registerPassword_;
                const std::string repeatedPassword = registerPasswordRepeat_;

                if (userName.empty()) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Login is required");
                } else if (password.empty()) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Password is required");
                } else if (password != repeatedPassword) {
                    copyToBuffer(authMessage_, sizeof(authMessage_), "Passwords do not match");
                } else {
                    const int userIndex = userManager_.registerUser(userName, displayName, password);
                    if (userIndex < 0) {
                        copyToBuffer(authMessage_, sizeof(authMessage_), "Login is already used");
                    } else {
                        selectedUserIndex_ = userIndex;
                        applyCurrentUser();
                        syncProfileBuffers();
                        refreshMazeList();
                        screen_ = Screen::Main;
                        copyToBuffer(loginName_, sizeof(loginName_), userName);
                        copyToBuffer(loginPassword_, sizeof(loginPassword_), password);
                        copyToBuffer(registerName_, sizeof(registerName_), "");
                        copyToBuffer(registerDisplayName_, sizeof(registerDisplayName_), "");
                        copyToBuffer(registerPassword_, sizeof(registerPassword_), "");
                        copyToBuffer(registerPasswordRepeat_, sizeof(registerPasswordRepeat_), "");
                        copyToBuffer(authMessage_, sizeof(authMessage_), "");
                        AppLogger::getInstance().log(LogLevel::INFO, "New user registered: " + userName);
                    }
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::TextWrapped("%s", authMessage_);
    ImGui::End();
}

void MazeApp::renderMainInterface() {
    ImGui::SetNextWindowPos({0.0F, 0.0F}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({380.0F, ImGui::GetIO().DisplaySize.y}, ImGuiCond_Always);
    ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("User")) {
            renderUserTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Generation")) {
            renderGenerationTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mazes")) {
            renderMazesTab();
            ImGui::EndTabItem();
        }
        if (controller_.canModerate() && ImGui::BeginTabItem("Users")) {
            renderUsersTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    renderStatusPanel();
    ImGui::End();
}

void MazeApp::renderUserTab() {
    const UserAccount* user = currentUser();
    if (user == nullptr) {
        return;
    }

    ImGui::Text("Login: %s", user->name.c_str());
    ImGui::Text("Role: %s", roleName(user->role));
    ImGui::Text("State: %s", user->blocked ? "blocked" : "active");
    ImGui::InputText("Display name", profileDisplayName_, sizeof(profileDisplayName_));
    ImGui::InputText("New password", profilePassword_, sizeof(profilePassword_), ImGuiInputTextFlags_Password);

    if (ImGui::Button("Save profile", {-1.0F, 0.0F})) {
        if (userManager_.updateProfile(selectedUserIndex_, profileDisplayName_, profilePassword_)) {
            applyCurrentUser();
            syncProfileBuffers();
            AppLogger::getInstance().log(LogLevel::INFO, "User profile updated");
        }
    }

    if (ImGui::Button("Logout", {-1.0F, 0.0F})) {
        logout();
    }
}

void MazeApp::renderGenerationTab() {
    int genIndex = generatorIndex(generatorType_);
    ImGui::Combo("Generator", &genIndex, "DFS\0Kruskal\0");
    generatorType_ = generatorFromIndex(genIndex);

    int pathIndex = pathfinderIndex(pathfinderType_);
    ImGui::Combo("Pathfinder", &pathIndex, "BFS\0A*\0");
    pathfinderType_ = pathfinderFromIndex(pathIndex);

    ImGui::SliderInt("Width", &width_, 5, 101);
    ImGui::SliderInt("Height", &height_, 5, 101);

    ImGui::Separator();
    ImGui::Text("Settings");
    float animationMs = animationStepSeconds_ * 1000.0F;
    if (ImGui::SliderFloat("Step ms", &animationMs, 1.0F, 80.0F, "%.0f")) {
        animationStepSeconds_ = animationMs / 1000.0F;
    }
    ImGui::Checkbox("Animate generation", &animateGeneration_);
    ImGui::Checkbox("Reveal carving", &revealGeneration_);
    ImGui::Checkbox("Animate search", &animateSearch_);

    ImGui::Separator();
    if (!controller_.canCreateMaze()) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Generate maze", {-1.0F, 0.0F})) {
        if (controller_.generateMaze(generatorType_, width_, height_)) {
            startSelected_ = false;
            finishSelected_ = false;
            visiblePath_ = 0;
            if (animateGeneration_) {
                startAnimation(AnimationMode::Generation);
            } else {
                visibleVisited_ = controller_.lastResult().visited.size();
                stopAnimation();
            }
        }
    }
    if (!controller_.canCreateMaze()) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    int modeIndex = selectionMode_ == SelectionMode::Start ? 0 : 1;
    ImGui::RadioButton("Click sets start", &modeIndex, 0);
    ImGui::RadioButton("Click sets finish", &modeIndex, 1);
    selectionMode_ = modeIndex == 0 ? SelectionMode::Start : SelectionMode::Finish;

    const bool noMaze = controller_.grid().empty();
    if (noMaze) {
        ImGui::BeginDisabled();
    }

    int startInput[2] = {controller_.start().x, controller_.start().y};
    if (ImGui::InputInt2("Start x/y", startInput)) {
        if (controller_.setStart({startInput[0], startInput[1]})) {
            startSelected_ = true;
            visibleVisited_ = 0;
            visiblePath_ = 0;
            stopAnimation();
        }
    }

    int finishInput[2] = {controller_.finish().x, controller_.finish().y};
    if (ImGui::InputInt2("Finish x/y", finishInput)) {
        if (controller_.setFinish({finishInput[0], finishInput[1]})) {
            finishSelected_ = true;
            visibleVisited_ = 0;
            visiblePath_ = 0;
            stopAnimation();
        }
    }

    if (noMaze) {
        ImGui::EndDisabled();
    }

    if (!canRunPathSearch()) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Find path", {-1.0F, 0.0F})) {
        if (controller_.findPath(pathfinderType_)) {
            if (animateSearch_) {
                startAnimation(AnimationMode::Search);
            } else {
                visibleVisited_ = controller_.lastResult().visited.size();
                visiblePath_ = controller_.lastResult().path.size();
                stopAnimation();
            }
        }
    }
    if (!canRunPathSearch()) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    ImGui::InputText("Save name", mazeName_, sizeof(mazeName_));
    ImGui::Checkbox("Published", &publishOnSave_);
    if (!controller_.canSaveMaze() || controller_.grid().empty()) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Save maze", {-1.0F, 0.0F})) {
        if (controller_.saveCurrentMaze(mazeName_, publishOnSave_)) {
            refreshMazeList();
        }
    }
    if (!controller_.canSaveMaze() || controller_.grid().empty()) {
        ImGui::EndDisabled();
    }
}

void MazeApp::renderMazesTab() {
    if (ImGui::Button("Refresh", {-1.0F, 0.0F})) {
        refreshMazeList();
    }

    ImGui::BeginChild("MazeLibrary", {0.0F, 360.0F}, ImGuiChildFlags_Border);
    for (const MazeRecord& record : mazeRecords_) {
        ImGui::PushID(record.id);
        ImGui::Text("#%d %s", record.id, record.name.c_str());
        ImGui::Text("%dx%d, %s, %s", record.width, record.height, record.owner.c_str(),
                    record.published ? "public" : "private");

        if (ImGui::Button("Open")) {
            if (controller_.loadMaze(record.id)) {
                startSelected_ = false;
                finishSelected_ = false;
                visibleVisited_ = 0;
                visiblePath_ = 0;
                stopAnimation();
            }
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

void MazeApp::renderUsersTab() {
    for (const UserAccount& user : userManager_.users()) {
        ImGui::PushID(user.name.c_str());
        ImGui::Text("%s / %s / %s", user.name.c_str(), roleName(user.role), user.displayName.c_str());
        ImGui::Text("State: %s", user.blocked ? "blocked" : "active");
        if (user.role != UserRole::Admin) {
            if (ImGui::Button(user.blocked ? "Unblock" : "Block")) {
                userManager_.setBlocked(user.name, !user.blocked);
                applyCurrentUser();
            }
        }
        ImGui::Separator();
        ImGui::PopID();
    }
}

void MazeApp::renderStatusPanel() {
    const AlgorithmResult& result = controller_.lastResult();
    ImGui::Separator();
    ImGui::TextWrapped("Status: %s", controller_.statusMessage().c_str());
    ImGui::Text("Start: %s", startSelected_ ? "selected" : "not selected");
    ImGui::Text("Finish: %s", finishSelected_ ? "selected" : "not selected");
    ImGui::Text("Visited: %zu", result.metrics.visitedCells);
    ImGui::Text("Path: %zu", result.metrics.pathLength);
    ImGui::Text("Time: %.3f ms", result.metrics.elapsedMs);
}

void MazeApp::refreshMazeList() {
    mazeRecords_ = controller_.listMazes();
}

void MazeApp::applyCurrentUser() {
    const UserAccount* user = currentUser();
    if (user == nullptr) {
        return;
    }

    controller_.setAccessContext(user->name, user->role, user->blocked);
}

void MazeApp::syncProfileBuffers() {
    const UserAccount* user = currentUser();
    if (user == nullptr) {
        return;
    }

    copyToBuffer(profileDisplayName_, sizeof(profileDisplayName_), user->displayName);
    copyToBuffer(profilePassword_, sizeof(profilePassword_), user->password);
}

void MazeApp::logout() {
    const UserAccount* user = currentUser();
    if (user != nullptr) {
        copyToBuffer(loginName_, sizeof(loginName_), user->name);
        copyToBuffer(loginPassword_, sizeof(loginPassword_), user->password);
    }

    selectedUserIndex_ = -1;
    screen_ = Screen::Login;
    startSelected_ = false;
    finishSelected_ = false;
    visibleVisited_ = 0;
    visiblePath_ = 0;
    stopAnimation();
    copyToBuffer(authMessage_, sizeof(authMessage_), "Signed out");
}

void MazeApp::startAnimation(AnimationMode mode) {
    visibleVisited_ = 0;
    visiblePath_ = 0;
    animationAccumulator_ = 0.0F;
    animationMode_ = mode;
}

void MazeApp::stopAnimation() {
    animationAccumulator_ = 0.0F;
    animationMode_ = AnimationMode::None;
}

void MazeApp::advanceAnimation(float deltaSeconds) {
    if (animationMode_ == AnimationMode::None) {
        return;
    }

    const AlgorithmResult& result = controller_.lastResult();
    animationAccumulator_ += deltaSeconds;
    while (animationAccumulator_ >= animationStepSeconds_) {
        animationAccumulator_ -= animationStepSeconds_;

        if (visibleVisited_ < result.visited.size()) {
            ++visibleVisited_;
        } else if (animationMode_ == AnimationMode::Search && visiblePath_ < result.path.size()) {
            ++visiblePath_;
        } else {
            stopAnimation();
            break;
        }
    }
}

bool MazeApp::canRunPathSearch() const {
    const UserAccount* user = currentUser();
    return user != nullptr && !user->blocked && !controller_.grid().empty() && startSelected_ && finishSelected_ &&
           controller_.start() != controller_.finish();
}

const UserAccount* MazeApp::currentUser() const {
    const auto& users = userManager_.users();
    if (selectedUserIndex_ < 0 || selectedUserIndex_ >= static_cast<int>(users.size())) {
        return nullptr;
    }
    return &users[static_cast<std::size_t>(selectedUserIndex_)];
}
