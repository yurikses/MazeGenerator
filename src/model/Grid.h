#pragma once
#include "Cell.h"
#include <vector>
struct Grid {
    int W = 0, H = 0;
    std::vector<std::vector<Cell>> data;
    Grid(int w = 0, int h = 0) : W(w), H(h), data(h, std::vector<Cell>(w)) {}
    void resize(int w, int h);
    void reset();
    Cell& at(int x, int y);
    const Cell& at(int x, int y) const;
};
