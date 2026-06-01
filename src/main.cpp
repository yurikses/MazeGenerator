#include "controller/MazeController.h"
#include "core/MazeStorage.h"
#include "patterns/factory.h"
#include "ui/MazeApp.h"

#include <exception>
#include <iostream>
#include <string>

namespace {
struct CliOptions {
    bool cli{false};
    bool findPath{true};
    int width{21};
    int height{21};
    AlgorithmType generator{AlgorithmType::DFSGenerator};
    AlgorithmType pathfinder{AlgorithmType::BFSPathfinder};
};

void printUsage(const char* executable) {
    std::cout << "Usage:\n"
              << "  " << executable << "                         Start GUI\n"
              << "  " << executable << " --cli [options]          Generate maze in console\n\n"
              << "Options:\n"
              << "  --width <n>                 Maze width, clamped to 5..101\n"
              << "  --height <n>                Maze height, clamped to 5..101\n"
              << "  --generator <dfs|kruskal>   Generation algorithm\n"
              << "  --pathfinder <bfs|astar>    Path search algorithm\n"
              << "  --no-path                   Skip path search\n"
              << "  --help                      Show this message\n";
}

bool parseAlgorithm(const std::string& value, AlgorithmType& target) {
    if (value == "dfs") {
        target = AlgorithmType::DFSGenerator;
        return true;
    }
    if (value == "kruskal") {
        target = AlgorithmType::KruskalGenerator;
        return true;
    }
    if (value == "bfs") {
        target = AlgorithmType::BFSPathfinder;
        return true;
    }
    if (value == "astar" || value == "a*") {
        target = AlgorithmType::AStarPathfinder;
        return true;
    }
    return false;
}

bool parseOptions(int argc, char** argv, CliOptions& options) {
    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];
        auto nextValue = [&]() -> std::string {
            if (index + 1 >= argc) {
                throw std::invalid_argument("Missing value for " + arg);
            }
            return argv[++index];
        };

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return false;
        }
        if (arg == "--cli") {
            options.cli = true;
        } else if (arg == "--width") {
            options.cli = true;
            options.width = std::stoi(nextValue());
        } else if (arg == "--height") {
            options.cli = true;
            options.height = std::stoi(nextValue());
        } else if (arg == "--generator") {
            options.cli = true;
            AlgorithmType parsed{};
            if (!parseAlgorithm(nextValue(), parsed) || !AlgorithmFactory::isGenerator(parsed)) {
                throw std::invalid_argument("Generator must be dfs or kruskal");
            }
            options.generator = parsed;
        } else if (arg == "--pathfinder") {
            options.cli = true;
            AlgorithmType parsed{};
            if (!parseAlgorithm(nextValue(), parsed) || !AlgorithmFactory::isPathfinder(parsed)) {
                throw std::invalid_argument("Pathfinder must be bfs or astar");
            }
            options.pathfinder = parsed;
        } else if (arg == "--no-path") {
            options.cli = true;
            options.findPath = false;
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    return true;
}

char cellGlyph(CellState state) {
    switch (state) {
        case CellState::Wall:
            return '#';
        case CellState::Path:
            return '.';
        case CellState::Start:
            return 'S';
        case CellState::Finish:
            return 'F';
        case CellState::Visited:
        case CellState::Frontier:
        case CellState::Passage:
            return ' ';
    }
    return '?';
}

void printGrid(const Grid& grid) {
    for (const auto& row : grid) {
        for (CellState cell : row) {
            std::cout << cellGlyph(cell);
        }
        std::cout << '\n';
    }
}

int runCli(const CliOptions& options) {
    MazeStorage storage{":memory:"};
    MazeController controller{storage};
    controller.setAccessContext("docker", UserRole::User, false);

    if (!controller.generateMaze(options.generator, options.width, options.height)) {
        std::cerr << controller.statusMessage() << '\n';
        return 1;
    }
    if (options.findPath && !controller.findPath(options.pathfinder)) {
        std::cerr << controller.statusMessage() << '\n';
        return 1;
    }

    const AlgorithmResult& result = controller.lastResult();
    std::cout << "Maze: " << gridWidth(controller.grid()) << "x" << gridHeight(controller.grid()) << '\n'
              << "Generator: " << algorithmName(options.generator) << '\n';
    if (options.findPath) {
        std::cout << "Pathfinder: " << algorithmName(options.pathfinder) << '\n'
                  << "Visited: " << result.metrics.visitedCells << '\n'
                  << "Path length: " << result.metrics.pathLength << '\n';
    }
    printGrid(controller.grid());
    return 0;
}
} // namespace

int main(int argc, char** argv) {
    CliOptions options;
    try {
        if (!parseOptions(argc, argv, options)) {
            return 0;
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\n\n";
        printUsage(argv[0]);
        return 2;
    }

    if (options.cli) {
        return runCli(options);
    }

    MazeApp app;
    return app.run();
}
