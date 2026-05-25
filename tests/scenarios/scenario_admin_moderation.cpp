#include "controller/MazeController.h"
#include "core/UserManager.h"

#include <iostream>

int main() {
    UserManager users;
    MazeStorage storage(":memory:");
    MazeController controller(storage);

    controller.setAccessContext("student", UserRole::User, false);
    if (!controller.generateMaze(AlgorithmType::DFSGenerator, 15, 15)) {
        std::cerr << "generation failed\n";
        return 1;
    }
    if (!controller.saveCurrentMaze("moderated private maze", false)) {
        std::cerr << "save failed\n";
        return 2;
    }

    controller.setAccessContext("admin", UserRole::Admin, false);
    const auto records = controller.listMazes();
    if (records.size() != 1 || records.front().published) {
        std::cerr << "admin listing failed\n";
        return 3;
    }
    if (!controller.deleteMaze(records.front().id) || !controller.listMazes().empty()) {
        std::cerr << "delete failed\n";
        return 4;
    }
    if (!users.setBlocked("student", true) || !users.users()[1].blocked) {
        std::cerr << "block failed\n";
        return 5;
    }
    if (!users.setBlocked("student", false) || users.users()[1].blocked) {
        std::cerr << "unblock failed\n";
        return 6;
    }

    std::cout << "ScenarioAdminModeration passed\n";
    return 0;
}
