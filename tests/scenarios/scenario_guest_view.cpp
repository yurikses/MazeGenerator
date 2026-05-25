#include "controller/MazeController.h"

#include <iostream>

int main() {
    MazeStorage storage(":memory:");
    MazeController owner(storage);
    owner.setAccessContext("student", UserRole::User, false);

    if (!owner.generateMaze(AlgorithmType::KruskalGenerator, 17, 17)) {
        std::cerr << "generation failed\n";
        return 1;
    }
    if (!owner.saveCurrentMaze("published", true) || !owner.saveCurrentMaze("private", false)) {
        std::cerr << "save failed\n";
        return 2;
    }

    MazeController guest(storage);
    guest.setAccessContext("guest", UserRole::Guest, false);
    const auto visible = guest.listMazes();
    if (visible.size() != 1 || !visible.front().published) {
        std::cerr << "guest visibility failed\n";
        return 3;
    }
    if (!guest.loadMaze(visible.front().id)) {
        std::cerr << "guest load failed\n";
        return 4;
    }
    if (!guest.findPath(AlgorithmType::AStarPathfinder)) {
        std::cerr << "guest path visualization failed\n";
        return 5;
    }

    std::cout << "ScenarioGuestView passed\n";
    return 0;
}
