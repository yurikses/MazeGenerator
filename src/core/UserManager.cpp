#include "core/UserManager.h"

#include <algorithm>
#include <cstddef>

UserManager::UserManager()
    : users_{
          {"guest", UserRole::Guest, false, "Guest", ""},
          {"student", UserRole::User, false, "Student", "student"},
          {"admin", UserRole::Admin, false, "Administrator", "admin"},
      } {}

const std::vector<UserAccount>& UserManager::users() const {
    return users_;
}

int UserManager::authenticate(const std::string& userName, const std::string& password) const {
    for (int index = 0; index < static_cast<int>(users_.size()); ++index) {
        const UserAccount& user = users_[static_cast<std::size_t>(index)];
        if (user.name == userName && user.password == password) {
            return index;
        }
    }
    return -1;
}

int UserManager::registerUser(const std::string& userName, const std::string& displayName, const std::string& password) {
    if (userName.empty() || password.empty()) {
        return -1;
    }
    if (std::any_of(users_.begin(), users_.end(), [&](const UserAccount& user) { return user.name == userName; })) {
        return -1;
    }

    const std::string finalDisplayName = displayName.empty() ? userName : displayName;
    users_.push_back({userName, UserRole::User, false, finalDisplayName, password});
    return static_cast<int>(users_.size()) - 1;
}

bool UserManager::updateProfile(int userIndex, const std::string& displayName, const std::string& password) {
    if (userIndex < 0 || userIndex >= static_cast<int>(users_.size())) {
        return false;
    }

    UserAccount& user = users_[static_cast<std::size_t>(userIndex)];
    if (!displayName.empty()) {
        user.displayName = displayName;
    }
    user.password = password;
    return true;
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
