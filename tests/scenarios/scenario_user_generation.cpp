#include "controller/MazeController.h"
#include "core/UserManager.h"

#include <iostream>

int main() {
    UserManager users;
    const int userIndex = users.registerUser("scenario_user", "Scenario User", "secret");
    if (userIndex < 0 || users.authenticate("scenario_user", "secret") != userIndex) {
        std::cerr << "registration failed\n";
        return 1;
    }

    MazeStorage storage(":memory:");
    MazeController controller(storage);
    const UserAccount& user = users.users()[static_cast<std::size_t>(userIndex)];
    controller.setAccessContext(user.name, user.role, user.blocked);

    if (!controller.generateMaze(AlgorithmType::DFSGenerator, 21, 21)) {
        std::cerr << "generation failed\n";
        return 2;
    }
    if (!controller.setStart(controller.start()) || !controller.setFinish(controller.finish())) {
        std::cerr << "endpoint selection failed\n";
        return 3;
    }
    if (!controller.findPath(AlgorithmType::BFSPathfinder)) {
        std::cerr << "path search failed\n";
        return 4;
    }
    if (!controller.saveCurrentMaze("scenario maze", true) || controller.listMazes().empty()) {
        std::cerr << "save failed\n";
        return 5;
    }

    std::cout << "ScenarioUserGeneration passed\n";
    return 0;
}
