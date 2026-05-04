import os

files = {
    "CMakeLists.txt": """cmake_minimum_required(VERSION 3.15)
project(MazeSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: Out-of-source build is recommended.
# mkdir build && cd build && cmake .. && cmake --build .

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/core/Cell.cpp
    src/core/Grid.cpp
    src/core/DFSGenerator.cpp
    src/core/KruskalGenerator.cpp
    src/core/AStarFinder.cpp
    src/core/BFSFinder.cpp
    src/ui/MazeView.cpp
    src/ui/ControlPanel.cpp
    src/ui/MazeController.cpp
    src/data/User.cpp
    src/data/UserManager.cpp
    src/data/MazeRepository.cpp
    src/utils/AppLogger.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
""",
    "README.md": """# MazeSystem

Генератор лабиринтов (DFS, Kruskal) + поиск пути (A*, BFS) с визуализацией и ролевой моделью.

## Зависимости
- C++17
- SFML 2.5+
- CMake 3.15+

## Инструкция по сборке
```sh
mkdir build && cd build && cmake .. && cmake --build .
```

## Пример запуска
```sh
./MazeSystem
```
""",
    ".gitignore": """build/
.vs/
out/
*.exe
""",
    "include/core/IMazeAlgorithm.h": """#pragma once
namespace maze::core {
    class IMazeAlgorithm {
    public:
        virtual ~IMazeAlgorithm() = default;
        virtual void generate() = 0;
    };
}
""",
    "include/core/DFSGenerator.h": """#pragma once
#include "IMazeAlgorithm.h"
namespace maze::core {
    class DFSGenerator : public IMazeAlgorithm {
    public:
        void generate() override;
    };
}
""",
    "src/core/DFSGenerator.cpp": """#include "core/DFSGenerator.h"
namespace maze::core {
    void DFSGenerator::generate() {
        // TODO: implement
    }
}
""",
    "include/core/KruskalGenerator.h": """#pragma once
#include "IMazeAlgorithm.h"
namespace maze::core {
    class KruskalGenerator : public IMazeAlgorithm {
    public:
        void generate() override;
    };
}
""",
    "src/core/KruskalGenerator.cpp": """#include "core/KruskalGenerator.h"
namespace maze::core {
    void KruskalGenerator::generate() {
        // TODO: implement
    }
}
""",
    "include/core/IPathfinder.h": """#pragma once
namespace maze::core {
    class IPathfinder {
    public:
        virtual ~IPathfinder() = default;
        virtual void findPath() = 0;
    };
}
""",
    "include/core/AStarFinder.h": """#pragma once
#include "IPathfinder.h"
namespace maze::core {
    class AStarFinder : public IPathfinder {
    public:
        void findPath() override;
    };
}
""",
    "src/core/AStarFinder.cpp": """#include "core/AStarFinder.h"
namespace maze::core {
    void AStarFinder::findPath() {
        // TODO: implement
    }
}
""",
    "include/core/BFSFinder.h": """#pragma once
#include "IPathfinder.h"
namespace maze::core {
    class BFSFinder : public IPathfinder {
    public:
        void findPath() override;
    };
}
""",
    "src/core/BFSFinder.cpp": """#include "core/BFSFinder.h"
namespace maze::core {
    void BFSFinder::findPath() {
        // TODO: implement
    }
}
""",
    "include/core/Grid.h": """#pragma once
namespace maze::core {
    class Grid {
    public:
        Grid();
        ~Grid();
    };
}
""",
    "src/core/Grid.cpp": """#include "core/Grid.h"
namespace maze::core {
    Grid::Grid() {}
    Grid::~Grid() {}
}
""",
    "include/core/Cell.h": """#pragma once
namespace maze::core {
    class Cell {
    public:
        Cell();
        ~Cell();
    };
}
""",
    "src/core/Cell.cpp": """#include "core/Cell.h"
namespace maze::core {
    Cell::Cell() {}
    Cell::~Cell() {}
}
""",
    "include/ui/MazeView.h": """#pragma once
namespace maze::ui {
    class MazeView {
    public:
        MazeView();
        ~MazeView();
    };
}
""",
    "src/ui/MazeView.cpp": """#include "ui/MazeView.h"
namespace maze::ui {
    MazeView::MazeView() {}
    MazeView::~MazeView() {}
}
""",
    "include/ui/ControlPanel.h": """#pragma once
namespace maze::ui {
    class ControlPanel {
    public:
        ControlPanel();
        ~ControlPanel();
    };
}
""",
    "src/ui/ControlPanel.cpp": """#include "ui/ControlPanel.h"
namespace maze::ui {
    ControlPanel::ControlPanel() {}
    ControlPanel::~ControlPanel() {}
}
""",
    "include/ui/MazeController.h": """#pragma once
namespace maze::ui {
    class MazeController {
    public:
        MazeController();
        ~MazeController();
    };
}
""",
    "src/ui/MazeController.cpp": """#include "ui/MazeController.h"
namespace maze::ui {
    MazeController::MazeController() {}
    MazeController::~MazeController() {}
}
""",
    "include/data/User.h": """#pragma once
namespace maze::data {
    class User {
    public:
        User();
        ~User();
    };
}
""",
    "src/data/User.cpp": """#include "data/User.h"
namespace maze::data {
    User::User() {}
    User::~User() {}
}
""",
    "include/data/UserManager.h": """#pragma once
namespace maze::data {
    class UserManager {
    public:
        UserManager();
        ~UserManager();
    };
}
""",
    "src/data/UserManager.cpp": """#include "data/UserManager.h"
namespace maze::data {
    UserManager::UserManager() {}
    UserManager::~UserManager() {}
}
""",
    "include/data/MazeRepository.h": """#pragma once
namespace maze::data {
    class MazeRepository {
    public:
        MazeRepository();
        ~MazeRepository();
    };
}
""",
    "src/data/MazeRepository.cpp": """#include "data/MazeRepository.h"
namespace maze::data {
    MazeRepository::MazeRepository() {}
    MazeRepository::~MazeRepository() {}
}
""",
    "include/utils/AlgorithmFactory.h": """#pragma once
namespace maze::utils {
    class AlgorithmFactory {
    public:
        AlgorithmFactory() = default;
        virtual ~AlgorithmFactory() = default;
    };
}
""",
    "include/utils/AppLogger.h": """#pragma once
namespace maze::utils {
    class AppLogger {
    public:
        AppLogger();
        ~AppLogger();
    };
}
""",
    "src/utils/AppLogger.cpp": """#include "utils/AppLogger.h"
namespace maze::utils {
    AppLogger::AppLogger() {}
    AppLogger::~AppLogger() {}
}
""",
    "include/utils/ProgressObserver.h": """#pragma once
namespace maze::utils {
    class ProgressObserver {
    public:
        virtual ~ProgressObserver() = default;
        virtual void updateProgress(int percent) = 0;
    };
}
""",
    "src/main.cpp": """#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "MazeSystem");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        // TODO: Draw maze
        window.display();
    }

    return 0;
}
"""
}

for filepath, content in files.items():
    with open(filepath, "w", encoding="utf-8") as f:
        f.write(content)

print("Files created successfully.")
