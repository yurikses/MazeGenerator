#include "Grid.h"
void Grid::resize(int w, int h) { W = w; H = h; data.assign(H, std::vector<Cell>(W)); }
void Grid::reset() { for(auto& r : data) for(auto& c : r) c = Cell{}; }
Cell& Grid::at(int x, int y) { return data[y][x]; }
const Cell& Grid::at(int x, int y) const { return data[y][x]; }
