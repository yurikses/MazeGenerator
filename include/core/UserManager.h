#pragma once

#include "core/types.h"

#include <string>
#include <vector>

class UserManager {
public:
    UserManager();

    const std::vector<UserAccount>& users() const;
    bool setBlocked(const std::string& userName, bool blocked);

    static bool canCreateMaze(UserRole role, bool blocked);
    static bool canSaveMaze(UserRole role, bool blocked);
    static bool canModerate(UserRole role, bool blocked);

private:
    std::vector<UserAccount> users_;
};
