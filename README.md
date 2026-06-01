# Генератор лабиринтов и поиск пути

## Возможности

- Экран авторизации со входом, гостевым режимом и регистрацией.
- Вкладка пользователя для изменения отображаемого имени, пароля и выхода из аккаунта.
- Вкладка генерации с выбором алгоритмов генерации и поиска пути, размера лабиринта, настроек анимации, параметров сохранения, начальной и конечной точек.
- Вкладка лабиринтов для открытия сохранённых лабиринтов.
- Вкладка пользователей для модерации аккаунтов администратором.
- Генерация лабиринтов алгоритмами DFS и Kruskal.
- Поиск пути алгоритмами BFS и A*.
- Отрисовка через SFML с элементами управления ImGui.
- Хранение сохранённых и опубликованных лабиринтов в SQLite.
- Логирование действий через singleton-логгер.

Демонстрационные аккаунты:

- `student` / `student`
- `admin` / `admin`
- новые аккаунты можно создать на вкладке `Register` на экране авторизации.

## Сборка

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Модульные тесты собираются в `build/tests/Debug`.
Сценарные программы собираются в `build/tests/scenarios/Debug`.
Список сценариев описан в `tests/scenarios/SCENARIOS.md`.

## Запуск

```powershell
.\build\bin\Debug\MazeApp.exe
```

Приложение также поддерживает консольный режим для запуска в чистом контейнерном окружении:

```powershell
.\build\bin\Debug\MazeApp.exe --cli --width 21 --height 15 --generator dfs --pathfinder bfs
```

Доступные CLI-алгоритмы: `dfs` или `kruskal` для генерации, `bfs` или `astar` для поиска пути.

## Docker

Docker-образ использует многостадийную Linux-сборку. На стадии сборки явно устанавливаются компилятор, CMake, Ninja, Git и системные Linux-библиотеки, необходимые SFML. Зависимости проекта, объявленные в CMake, загружаются во время конфигурации, затем выполняется полный набор тестов CTest, и только после этого создаётся runtime-образ.

Собрать итоговый образ:

```powershell
docker build -t maze-generator .
```

Запустить стандартную консольную демонстрацию:

```powershell
docker run --rm maze-generator
```

Передать аргументы командной строки для изменения генерируемого лабиринта:

```powershell
docker run --rm maze-generator --cli --width 31 --height 21 --generator kruskal --pathfinder astar
```

Запустить только тестовую стадию, если нужно проверить тесты внутри контейнера:

```powershell
docker build --target test -t maze-generator-test .
```

Запустить тесты в запущенном контейнере 
```powershell
docker run --rm maze-generator-test ctest --test-dir build --output-on-failure
```
