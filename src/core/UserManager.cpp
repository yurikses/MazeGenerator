#include "core/UserManager.h"

#include <algorithm>

UserManager::UserManager()
    : users_{
          {"guest", UserRole::Guest, false},
          {"student", UserRole::User, false},
          {"admin", UserRole::Admin, false},
      } {}

const std::vector<UserAccount>& UserManager::users() const {
    return users_;
}

bool UserManager::setBlocked(const std::string& userName, bool blocked) {
    auto it = std::find_if(users_.begin(), users_.end(), [&](const UserAccount& user) {
        return user.name == userName;
    });
    if (it == users_.end() || it->role == UserRole::Admin) {
        return false;
    }

    it->blocked = blocked;
    return true;
}

bool UserManager::canCreateMaze(UserRole role, bool blocked) {
    return !blocked && (role == UserRole::User || role == UserRole::Admin);
}

bool UserManager::canSaveMaze(UserRole role, bool blocked) {
    return canCreateMaze(role, blocked);
}

bool UserManager::canModerate(UserRole role, bool blocked) {
    return !blocked && role == UserRole::Admin;
}
