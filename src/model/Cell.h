#pragma once
struct Cell {
    bool visited = false;
    bool walls[4] = {true, true, true, true};
    int status = 0; // 0: normal, 1: open, 2: closed, 3: path
    int g = 0, h = 0, f = 0;
};
