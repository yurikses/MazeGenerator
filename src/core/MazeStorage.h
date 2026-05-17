#pragma once
#include "../model/MazeData.h"
#include <vector>
#include <optional>

class MazeStorage {
public:
    // загрузить все метаданные и грида
    static std::vector<MazeData> loadAll();
    // загрузить по id
    static std::optional<MazeData> loadById(const std::string& id);
    static void save(const MazeData& m);
    static void remove(const std::string& id);
};
