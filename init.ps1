# init.ps1
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$PROJECT = "MazeProject"
Write-Host "Creating project structure: $PROJECT" -ForegroundColor Cyan

$dirs = @(
    "include/core", "include/patterns", "include/algorithms", "include/controller", "include/ui",
    "src/core", "src/patterns", "src/algorithms", "src/controller", "src/ui",
    "tests/resources", "resources/fonts", "build"
)
foreach ($dir in $dirs) { New-Item -Path "$dir" -ItemType Directory -Force | Out-Null }

$files = @(
    "CMakeLists.txt", "README.md", "src/main.cpp",
    "include/core/Types.h", "include/core/AppLogger.h", "include/core/UserManager.h", "include/core/MazeStorage.h",
    "include/patterns/Strategy.h", "include/patterns/Observer.h", "include/patterns/Factory.h",
    "include/algorithms/DFSGenerator.h", "include/algorithms/KruskalGenerator.h",
    "include/algorithms/BFSFinder.h", "include/algorithms/AStarFinder.h",
    "include/controller/MazeController.h", "include/ui/MazeApp.h", "include/ui/GUIMazeView.h",
    "src/core/AppLogger.cpp", "src/core/UserManager.cpp", "src/core/MazeStorage.cpp",
    "src/patterns/Strategy.cpp", "src/patterns/Observer.cpp", "src/patterns/Factory.cpp",
    "src/algorithms/DFSGenerator.cpp", "src/algorithms/KruskalGenerator.cpp",
    "src/algorithms/BFSFinder.cpp", "src/algorithms/AStarFinder.cpp",
    "src/controller/MazeController.cpp", "src/ui/MazeApp.cpp", "src/ui/GUIMazeView.cpp",
    "tests/test_factory.cpp", "tests/test_algorithms.cpp", "tests/test_roles.cpp"
)
foreach ($file in $files) {
    $path = "$file"
    if (!(Test-Path $path)) { New-Item -Path $path -ItemType File -Force | Out-Null }
}

$gitignore = @"
build/
*.log
*.db
.vs/
.idea/
*.suo
*.user
*.sln
*.vcxproj.user
"@
Set-Content -Path "$PROJECT/.gitignore" -Value $gitignore -Encoding UTF8

Write-Host "Project structure '$PROJECT' created successfully!" -ForegroundColor Green
Write-Host "Open the folder in your IDE and start coding." -ForegroundColor Yellow