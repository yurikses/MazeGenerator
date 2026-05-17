#pragma once
#include "Cell.h"
#include <string>
#include <vector>
struct MazeData {
    std::string id, owner;
    int W, H;
    bool published = false;
    std::vector<std::vector<Cell>> grid;
};