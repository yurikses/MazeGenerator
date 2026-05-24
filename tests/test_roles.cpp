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

TEST(UserManager, AuthenticatesAndUpdatesProfile) {
    UserManager users;

    const int studentIndex = users.authenticate("student", "student");
    ASSERT_GE(studentIndex, 0);

    EXPECT_TRUE(users.updateProfile(studentIndex, "Updated Student", "newpass"));
    EXPECT_LT(users.authenticate("student", "student"), 0);
    EXPECT_EQ(users.authenticate("student", "newpass"), studentIndex);
    EXPECT_EQ(users.users()[static_cast<std::size_t>(studentIndex)].displayName, "Updated Student");
}

TEST(UserManager, RegistersNewUserAccounts) {
    UserManager users;

    const int newUserIndex = users.registerUser("new_user", "New User", "secret");
    ASSERT_GE(newUserIndex, 0);

    EXPECT_EQ(users.authenticate("new_user", "secret"), newUserIndex);
    EXPECT_EQ(users.users()[static_cast<std::size_t>(newUserIndex)].role, UserRole::User);
    EXPECT_FALSE(users.users()[static_cast<std::size_t>(newUserIndex)].blocked);
    EXPECT_LT(users.registerUser("new_user", "Duplicate", "secret"), 0);
    EXPECT_LT(users.registerUser("", "No Login", "secret"), 0);
    EXPECT_LT(users.registerUser("no_password", "No Password", ""), 0);
}
