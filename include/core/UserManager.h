#pragma once

#include "core/types.h"

#include <string>
#include <vector>

class UserManager {
public:
    UserManager();

    const std::vector<UserAccount>& users() const;
    int authenticate(const std::string& userName, const std::string& password) const;
    int registerUser(const std::string& userName, const std::string& displayName, const std::string& password);
    bool updateProfile(int userIndex, const std::string& displayName, const std::string& password);
    bool setBlocked(const std::string& userName, bool blocked);

    static bool canCreateMaze(UserRole role, bool blocked);
    static bool canSaveMaze(UserRole role, bool blocked);
    static bool canModerate(UserRole role, bool blocked);

private:
    std::vector<UserAccount> users_;
};
