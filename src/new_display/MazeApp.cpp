#include "MazeApp.h"
#include <SFML/Graphics.hpp>
#include "SaveMazeCommand.h"
#include "../core/MazeStorage.h"
#include "../model/MazeData.h"

MazeAppNew::MazeAppNew() : ctrl(40,30) {
    ctrl.setAlgorithm(AlgorithmFactory::create(AlgoTypeNew::DFS));
    ctrl.setObserver(std::shared_ptr<IObserver>(this, [](auto*){}));
}

void MazeAppNew::onStep(const VisData& d) {
    Grid &g = ctrl.getGrid();
    if(d.x<0 || d.y<0 || d.x>=g.W || d.y>=g.H) return;
    if(d.type==0) {
        g.at(d.x,d.y).visited = true;
        g.at(d.x,d.y).status = 1;
    } else if(d.type==1) {
        g.at(d.x,d.y).status = 3;
    }
}

void MazeAppNew::run() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Maze System");
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;
    sf::Clock stepClock;
    int speedMs = 20;

    std::vector<MazeData> mazeList;
    char loginBuf[64] = "user";
    char passBuf[64] = "123";
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Controls");
        if(!AuthManager::instance().isLoggedIn()) {
            ImGui::InputText("Login", loginBuf, sizeof(loginBuf));
            ImGui::InputText("Pass", passBuf, sizeof(passBuf));
            if(ImGui::Button("Register")) {
                AuthManager::instance().registerUser(loginBuf, passBuf);
            }
            ImGui::SameLine();
            if(ImGui::Button("Login")) {
                if(!AuthManager::instance().login(loginBuf, passBuf)) {
                    // simple feedback
                }
            }
            ImGui::Text("Not logged in");
        } else {
            ImGui::Text("Current: %s", AuthManager::instance().currentUser().c_str());
            if(ImGui::Button("Logout")) {
                AuthManager::instance().logout();
            }

            if (ImGui::BeginTabBar("MainTabs")) {
                if(ImGui::BeginTabItem("Generation")) {
                    static int algo = 0;
                    ImGui::Combo("Algorithm", &algo, "DFS\0KRUSKAL\0BFS\0ASTAR\0");
                    if (ImGui::Button("Set Algorithm")) {
                        ctrl.setAlgorithm(AlgorithmFactory::create((AlgoTypeNew)algo));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Run")) { ctrl.run(); }
                    ImGui::SameLine();
                    if (ImGui::Button("Step")) { ctrl.step(); }
                    ImGui::SameLine();
                    if (ImGui::Button("Stop")) { ctrl.stop(); }
                    ImGui::SliderInt("Speed ms", &speedMs, 1, 500);
                    ImGui::EndTabItem();
                }

                if(ImGui::BeginTabItem("User")) {
                    ImGui::Text("User: %s", AuthManager::instance().currentUser().c_str());
                    ImGui::Text("Settings:");
                    // future user settings here
                    ImGui::EndTabItem();
                }

                if(ImGui::BeginTabItem("Saved Mazes")) {
                    if(ImGui::Button("Refresh list")) {
                        mazeList = MazeStorage::loadAll();
                    }
                    ImGui::Separator();
                    for(auto &m: mazeList) {
                        ImGui::Text("%s by %s (%dx%d)", m.id.c_str(), m.owner.c_str(), m.W, m.H);
                        ImGui::SameLine();
                        if(ImGui::SmallButton((std::string("Load##") + m.id).c_str())) {
                            auto opt = MazeStorage::loadById(m.id);
                            if(opt) {
                                ctrl.resize(opt->W, opt->H);
                                ctrl.getGrid().data = opt->grid;
                            }
                        }
                        ImGui::SameLine();
                        if(ImGui::SmallButton((std::string("Delete##") + m.id).c_str())) {
                            MazeStorage::remove(m.id);
                        }
                    }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();

        // auto-stepping: if running, step according to speed
        if(ctrl.isRunning() && stepClock.getElapsedTime().asMilliseconds() >= speedMs) {
            ctrl.step();
            stepClock.restart();
        }

        window.clear(sf::Color::Black);
        Grid &g = ctrl.getGrid();
        const int cellSize = 16;
        sf::RectangleShape rect(sf::Vector2f({(float)cellSize-1,(float)cellSize-1}));
        for(int y=0;y<g.H;y++) for(int x=0;x<g.W;x++) {
            auto &c = g.at(x,y);
            sf::Color color = sf::Color(30,30,30);
            if(c.status==1) color = sf::Color::Green;
            if(c.status==2) color = sf::Color::Yellow;
            if(c.status==3) color = sf::Color::Red;
            if(c.visited) color = sf::Color(80,80,200);
            rect.setPosition(sf::Vector2f({(float)(x*cellSize+10), (float)(y*cellSize+10)}));
            rect.setFillColor(color);
            window.draw(rect);
            sf::RectangleShape wall;
            wall.setFillColor(sf::Color::White);
            if(c.walls[0]) {
                wall.setSize(sf::Vector2f({(float)cellSize, 2.f}));
                wall.setPosition(sf::Vector2f({(float)(x*cellSize+10), (float)(y*cellSize+10)}));
                window.draw(wall);
            }
            if(c.walls[1]) {
                wall.setSize(sf::Vector2f({2.f, (float)cellSize}));
                wall.setPosition(sf::Vector2f({(float)(x*cellSize+cellSize+10), (float)(y*cellSize+10)}));
                window.draw(wall);
            }
            if(c.walls[2]) {
                wall.setSize(sf::Vector2f({(float)cellSize, 2.f}));
                wall.setPosition(sf::Vector2f({(float)(x*cellSize+10), (float)(y*cellSize+cellSize+10)}));
                window.draw(wall);
            }
            if(c.walls[3]) {
                wall.setSize(sf::Vector2f({2.f, (float)cellSize}));
                wall.setPosition(sf::Vector2f({(float)(x*cellSize+10), (float)(y*cellSize+10)}));
                window.draw(wall);
            }
        }
        ImGui::SFML::Render(window);
        window.display();
        sf::sleep(sf::milliseconds(10));
        if(!window.isOpen()) break;
    }
    ImGui::SFML::Shutdown();
}

void MazeAppNew::executeCommand(std::unique_ptr<Command> cmd) {
    if(cmd) cmd->execute();
}