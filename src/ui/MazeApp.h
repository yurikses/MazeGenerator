#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../core/MazeController.h"
#include "../observer/IObserver.h"
#include "../model/MazeData.h" // <-- ДОБАВЛЕНО
#include <memory>
#include <vector>

class MazeApp : public IObserver {
    sf::RenderWindow win;
    MazeController ctrl;
    std::shared_ptr<IObserver> selfPtr;
    bool running = false, showAuth=true, showAdmin=false;
    char login[64] = "user";
    char pass[64] = "123";
    char newMazeId[64] = "maze_01";
    int mazeW=20, mazeH=15, speed=50;
    AlgoType genType=AlgoType::DFS, pathType=AlgoType::BFS;
    bool isPathMode = false;
    sf::Clock clk;
    std::vector<MazeData> mazeList;
public:
    MazeApp();
    void onStep(const VisData& d) override;
    void run();
    void uiAuth(); void uiMain(); void uiAdmin(); void drawMaze();
};
