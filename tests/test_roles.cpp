#include "core/UserManager.h"

#include <gtest/gtest.h>

TEST(UserManager, EnforcesRolePermissions) {
    EXPECT_FALSE(UserManager::canCreateMaze(UserRole::Guest, false));
    EXPECT_TRUE(UserManager::canCreateMaze(UserRole::User, false));
    EXPECT_TRUE(UserManager::canCreateMaze(UserRole::Admin, false));
    EXPECT_FALSE(UserManager::canCreateMaze(UserRole::User, true));

    EXPECT_FALSE(UserManager::canModerate(UserRole::User, false));
    EXPECT_TRUE(UserManager::canModerate(UserRole::Admin, false));
}

TEST(UserManager, BlocksNonAdminAccounts) {
    UserManager users;

    EXPECT_TRUE(users.setBlocked("student", true));
    EXPECT_FALSE(users.setBlocked("admin", true));
}
