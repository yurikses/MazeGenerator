cmake_minimum_required(VERSION 3.28)
project(maze LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endif()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)

set(SOURCES
    src/main.cpp
    src/model/Grid.cpp
    src/observer/Subject.cpp
    # (old algorithm implementations removed - using new_algo/*)
    src/core/AppLogger.cpp
    src/core/UserManager.cpp
    src/core/MazeStorage.cpp
    # (old core/ui implementations removed - using new_core/* and new_display/*)
    # new modules
    src/new_user/AuthManager.cpp
    src/new_algo/DFSGenerator.cpp
    src/new_algo/BFSFinder.cpp
    src/new_algo/AStarFinder.cpp
    src/new_algo/KruskalGenerator.cpp
    src/new_algo/AlgorithmFactory.cpp
    src/new_core/MazeController.cpp
    src/new_display/MazeApp.cpp
)

add_executable(maze ${SOURCES})
target_include_directories(maze PRIVATE src)
target_compile_features(maze PRIVATE cxx_std_17)
target_link_libraries(maze PRIVATE sfml-graphics sfml-window sfml-system ImGui-SFML::ImGui-SFML)
