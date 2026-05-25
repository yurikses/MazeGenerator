# Maze Generator & Path Finder

## Features

- Authorization screen with sign in, guest mode, and registration.
- User tab for display name/password changes and logout.
- Generation tab for generator/pathfinder selection, maze size, animation settings, save options, start/finish selection, generation, and path search.
- Mazes tab for opening saved mazes.
- Users tab for admin account moderation.
- Maze generation: DFS and Kruskal.
- Path search: BFS and A*.
- SFML rendering with ImGui controls.
- SQLite storage for saved and published mazes.
- Action logging through a singleton logger.

Demo accounts:

- `student` / `student`
- `admin` / `admin`
- new accounts can be created from the `Register` tab on the authorization screen.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Unit tests are built into `build/tests/Debug`.
Scenario programs are built into `build/tests/scenarios/Debug`.
The scenario list is documented in `tests/scenarios/SCENARIOS.md`.

## Run

```powershell
.\build\bin\Debug\MazeApp.exe
```
