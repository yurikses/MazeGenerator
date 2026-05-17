#include "MazeStorage.h"
#include <fstream>
#include <sstream>
#include <algorithm>
static std::string path() { return "mazes.dat"; }

// Формат (текстовый):
// id owner W H published\n
// затем W*H записей клеток: visited status walls (0/1) подряд

std::vector<MazeData> MazeStorage::loadAll() {
    std::vector<MazeData> res; std::ifstream f(path()); if(!f) return res;
    while(f) {
        MazeData m; if(!(f >> m.id >> m.owner >> m.W >> m.H >> m.published)) break;
        m.grid.assign(m.H, std::vector<Cell>(m.W));
        for(int y=0;y<m.H;y++) for(int x=0;x<m.W;x++) {
            int visited, status; f >> visited >> status;
            m.grid[y][x].visited = visited;
            m.grid[y][x].status = status;
            for(int k=0;k<4;k++) { int w; f >> w; m.grid[y][x].walls[k] = w; }
        }
        res.push_back(m);
    }
    return res;
}

std::optional<MazeData> MazeStorage::loadById(const std::string& id) {
    auto all = loadAll();
    for(auto &m: all) if(m.id==id) return m;
    return std::nullopt;
}

void MazeStorage::save(const MazeData& m) {
    std::ofstream f(path(), std::ios::app);
    if(!f) return;
    f << m.id << " " << m.owner << " " << m.W << " " << m.H << " " << m.published << "\n";
    for(int y=0;y<m.H;y++) for(int x=0;x<m.W;x++) {
        const Cell &c = m.grid[y][x];
        f << (c.visited?1:0) << " " << c.status << " ";
        for(int k=0;k<4;k++) f << (c.walls[k]?1:0) << " ";
        f << "\n";
    }
}

void MazeStorage::remove(const std::string& id) {
    auto all=loadAll(); all.erase(std::remove_if(all.begin(),all.end(),[&](const MazeData& m){return m.id==id;}),all.end());
    std::ofstream f(path(), std::ios::trunc);
    for(auto& m:all) {
        f << m.id << " " << m.owner << " " << m.W << " " << m.H << " " << m.published << "\n";
        for(int y=0;y<m.H;y++) for(int x=0;x<m.W;x++) {
            const Cell &c = m.grid[y][x];
            f << (c.visited?1:0) << " " << c.status << " ";
            for(int k=0;k<4;k++) f << (c.walls[k]?1:0) << " ";
            f << "\n";
        }
    }
}
