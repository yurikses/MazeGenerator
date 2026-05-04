cmake_minimum_required(VERSION 3.15)
project(MazeSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Note: Out-of-source build is recommended.
# mkdir build && cd build && cmake .. && cmake --build .

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

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
