# Maze Generator & Path Finder

C++ desktop application for generating mazes and visualizing path search.

## Features

- Maze generation: DFS and Kruskal.
- Path search: BFS and A*.
- SFML rendering with ImGui controls.
- Start and finish selection by mouse click or coordinate input.
- Animated visualization of visited cells and final path.
- Roles: Guest, User, Admin.
- SQLite storage for saved and published mazes.
- Action logging through a singleton logger.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## Run

```powershell
.\build\bin\Debug\MazeApp.exe
```
