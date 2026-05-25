cmake_minimum_required(VERSION 3.16)
project(MazeProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endif()

include(FetchContent)
include(CTest)
cmake_policy(SET CMP0135 NEW)

set(SFML_VERSION 3.0.1)
FetchContent_Declare(
    SFML
    URL "https://github.com/SFML/SFML/archive/refs/tags/${SFML_VERSION}.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
option(SFML_BUILD_AUDIO "Build SFML Audio module" OFF)
option(SFML_BUILD_NETWORK "Build SFML Network module" OFF)
FetchContent_MakeAvailable(SFML)

set(IMGUI_VERSION 1.91.1)
FetchContent_Declare(
    imgui
    URL "https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" OFF)
FetchContent_MakeAvailable(imgui-sfml)

FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450200.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite3)

if(NOT TARGET SQLite::SQLite3)
    add_library(SQLite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c ${sqlite3_SOURCE_DIR}/sqlite3.h)
    target_include_directories(SQLite3 PUBLIC ${sqlite3_SOURCE_DIR})
    target_compile_definitions(SQLite3 PUBLIC SQLITE_THREADSAFE=1 SQLITE_ENABLE_JSON1)
    add_library(SQLite::SQLite3 ALIAS SQLite3)
endif()

set(MAZE_CORE_SOURCES
    src/core/AppLogger.cpp
    src/core/UserManager.cpp
    src/core/MazeStorage.cpp
    src/patterns/Strategy.cpp
    src/patterns/Observer.cpp
    src/patterns/Factory.cpp
    src/algorithms/DFSGenerator.cpp
    src/algorithms/KruskalGenerator.cpp
    src/algorithms/BFSFinder.cpp
    src/algorithms/AStarFinder.cpp
    src/controller/MazeController.cpp
)

set(MAZE_CORE_HEADERS
    include/core/types.h
    include/core/AppLogger.h
    include/core/UserManager.h
    include/core/MazeStorage.h
    include/patterns/algorithm.h
    include/patterns/Strategy.h
    include/patterns/Observer.h
    include/patterns/factory.h
    include/algorithms/DFSGenerator.h
    include/algorithms/KruskalGenerator.h
    include/algorithms/BFSFinder.h
    include/algorithms/AStarFinder.h
    include/controller/MazeController.h
    include/services/app_logger.h
    include/services/maze_repository.h
)

add_library(MazeCore STATIC ${MAZE_CORE_SOURCES} ${MAZE_CORE_HEADERS})
target_include_directories(MazeCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(MazeCore PUBLIC cxx_std_17)
target_link_libraries(MazeCore PUBLIC SQLite::SQLite3)

add_executable(MazeApp
    src/main.cpp
    src/ui/MazeApp.cpp
    src/ui/GUIMazeView.cpp
    include/ui/MazeApp.h
    include/ui/GUIMazeView.h
)

target_include_directories(MazeApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(MazeApp PRIVATE cxx_std_17)
target_link_libraries(MazeApp PRIVATE
    MazeCore
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
)

foreach(target MazeCore MazeApp)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    endif()
endforeach()

add_custom_command(TARGET MazeApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/resources
    COMMENT "Copying resources to output directory"
)

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW ON
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(MazeTests
        tests/test_algorithms.cpp
        tests/test_factory.cpp
        tests/test_observer.cpp
        tests/test_storage.cpp
        tests/test_controller.cpp
        tests/test_gui_view.cpp
        tests/test_logger.cpp
        tests/test_types.cpp
        tests/test_roles.cpp
        src/ui/GUIMazeView.cpp
    )
    target_link_libraries(MazeTests PRIVATE MazeCore GTest::gtest_main sfml-graphics sfml-window sfml-system)
    target_include_directories(MazeTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set_target_properties(MazeTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )

    include(GoogleTest)
    gtest_discover_tests(MazeTests)

    function(add_maze_scenario target source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE MazeCore)
        target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/scenarios
        )
        add_test(NAME ${target} COMMAND ${target})
    endfunction()

    add_maze_scenario(ScenarioUserGeneration tests/scenarios/scenario_user_generation.cpp)
    add_maze_scenario(ScenarioGuestView tests/scenarios/scenario_guest_view.cpp)
    add_maze_scenario(ScenarioAdminModeration tests/scenarios/scenario_admin_moderation.cpp)
    add_maze_scenario(ScenarioAlgorithmComparison tests/scenarios/scenario_algorithm_comparison.cpp)
endif()

message(STATUS "MazeProject configured")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  SFML: ${SFML_VERSION}")
message(STATUS "  ImGui: ${IMGUI_VERSION}")
message(STATUS "  Tests: ${BUILD_TESTING}")
